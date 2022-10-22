#include "AI/Tasks/AMyTaskTakeCover.h"

#include "Controllers/AIControllerBase.h"
#include "Characters/AICharacter.h"
#include "AI/Actors/PlaceToCover.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UAMyTaskTakeCover::UAMyTaskTakeCover()
{
	bCreateNodeInstance = true;
	bNotifyTick = false;
	NodeName = "Take cover";
}

EBTNodeResult::Type UAMyTaskTakeCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	auto Pawn = Controller->GetPawn();
	if (Pawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	auto Cover = FindCover(OwnerComp);
	if (Cover == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Controller->ReceiveMoveCompleted.AddDynamic(this, &UAMyTaskTakeCover::OnMoveCompleted);
	auto Pos = Cover->NearestPosInCover(Pawn->GetActorLocation());
	Controller->MoveToLocation(Pos);
	CurrentCoverRequiresCrouching = Cover->NeedToCrouch;
	CurrentCoverOrientation = Cover->OrientationInPos(Pos);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UAMyTaskTakeCover::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UAMyTaskTakeCover::OnMoveCompleted);
	Controller->StopMovement();
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	if (auto Character = Cast<AAICharacter>(Controller->GetPawn()); Character != nullptr)
	{
		Character->Crouch(false);
	}
	return EBTNodeResult::Aborted;
}

void UAMyTaskTakeCover::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	check(Controller != nullptr);
	auto Pawn = Controller->GetPawn();
	if (auto Character = Cast<AAICharacter>(Pawn); Character != nullptr)
	{
		Character->Crouch(CurrentCoverRequiresCrouching);
	}
	if (Pawn != nullptr)
	{
		Controller->SetFocalPoint(Pawn->GetActorLocation() + CurrentCoverOrientation);
		auto DamageTakerComponent = Pawn->FindComponentByClass<UDamageTakerComponent>();
		check(DamageTakerComponent != nullptr);
		DamageTakerComponent->DamageTaken.AddDynamic(this, &UAMyTaskTakeCover::OnDamageTaken);
	}

	Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UAMyTaskTakeCover::OnMoveCompleted);

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle
		, this
		, &UAMyTaskTakeCover::TimeInCoverExpiredWithSuccess
		, TimeInCover
		, /*bInLoop*/ false
	);
}

void UAMyTaskTakeCover::TimeInCoverExpiredWithSuccess()
{
	TimeInCoverExpired(true);
}

void UAMyTaskTakeCover::TimeInCoverExpired(bool Succeeded)
{
	check(Controller != nullptr);
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	if (auto Character = Cast<AAICharacter>(Controller->GetPawn()); Character != nullptr)
	{
		Character->Crouch(false);
	}
	auto Tree = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
	check(Tree != nullptr);
	FinishLatentTask(*Tree, Succeeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
}

void UAMyTaskTakeCover::OnDamageTaken(FName BoneName, float Damage)
{
	check(Controller != nullptr);
	auto Pawn = Controller->GetPawn();
	auto DamageTakerComponent = Pawn->FindComponentByClass<UDamageTakerComponent>();
	check(DamageTakerComponent != nullptr);
	DamageTakerComponent->DamageTaken.RemoveDynamic(this, &UAMyTaskTakeCover::OnDamageTaken);

	TimeInCoverExpired(false);
}

class APlaceToCover* UAMyTaskTakeCover::FindCover(UBehaviorTreeComponent& OwnerComp) const
{
	static constexpr float MinCos = 0.7;

	FVector DamageDirection(1.f, 0.f, 0.f);
	AActor* EnemyActor = nullptr;
	if (auto Blackboard = OwnerComp.GetBlackboardComponent(); Blackboard != nullptr)
	{
		DamageDirection = -Blackboard->GetValueAsVector(Controller->LastDamageTakenDirectionKeyName);
		EnemyActor = Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName));
	}
	else
	{
		return nullptr;
	}

	auto Pawn = Controller->GetPawn();
	check(Pawn != nullptr);
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlaceToCover::StaticClass(), Actors);
	Actors.RemoveAllSwap(
		[this, EnemyActor, Pawn](const AActor* Actor)
			{
				auto Cover = Cast<APlaceToCover>(Actor);
				return
					Cover == nullptr
					|| (Cover->GetCenter() - Pawn->GetActorLocation()).Length() > Controller->RadiusOfCoverSearch
					|| EnemyActor != nullptr && (Cover->GetCenter() - EnemyActor->GetActorLocation()).Length() < Controller->MinDistanceFromCoverToEnemy
				;
			}
		, /*bAllowShrinking*/ true
	);

	const auto NavSys = UNavigationSystemV1::GetCurrent(Pawn);

	APlaceToCover* BestCover = nullptr;
	float BestCriterion = FLT_MAX;
	for (auto Actor : Actors)
	{
		auto Cover = Cast<APlaceToCover>(Actor);
		if (Cover == nullptr)
		{
			check(false);
			continue;
		}

		FVector CoverForward = (Cover->CoverPositionRange1->GetForwardVector() + Cover->CoverPositionRange2->GetForwardVector()).GetSafeNormal();
		auto Cos = FVector::DotProduct(DamageDirection, CoverForward); // the higher the better

		if (Cos < MinCos)
		{
			continue;
		}

		float Cost;
		auto Result =
			NavSys->GetPathCost(
				Pawn->GetActorLocation()
				, Cover->NearestPosInCover(Pawn->GetActorLocation())
				, Cost
			)
		;
		if (Result != ENavigationQueryResult::Success)
		{
			continue;
		}

		auto Criterion = Cost / Cos;

		if (Criterion < BestCriterion)
		{
			BestCover = Cover;
			BestCriterion = Criterion;
		}
	}

	return BestCover;
}
