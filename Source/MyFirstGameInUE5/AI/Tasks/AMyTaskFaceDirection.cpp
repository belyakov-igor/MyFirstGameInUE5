#include "AI/Tasks/AMyTaskFaceDirection.h"

#include "Controllers/AIControllerBase.h"
#include "Characters/AICharacter.h"
#include "AI/Actors/PlaceToCover.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UAMyTaskFaceDirection::UAMyTaskFaceDirection()
{
	bCreateNodeInstance = false;
	bNotifyTick = false;
	NodeName = "Face direction";

	DirectionKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UAMyTaskFaceDirection, DirectionKey));
}

void UAMyTaskFaceDirection::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		DirectionKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UAMyTaskFaceDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
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

	auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	auto Direction = Blackboard->GetValueAsVector(DirectionKey.SelectedKeyName);

	Controller->SetFocalPoint(Pawn->GetActorLocation() + Direction);

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle
		,
			[this]
			{
				check(Controller != nullptr);
				Controller->ClearFocus(EAIFocusPriority::Gameplay);
				auto Tree = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
				check(Tree != nullptr);
				FinishLatentTask(*Tree, EBTNodeResult::Succeeded);
			}
		, TimeInThisTask
		, /*bInLoop*/ false
	);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UAMyTaskFaceDirection::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	return EBTNodeResult::Aborted;
}
