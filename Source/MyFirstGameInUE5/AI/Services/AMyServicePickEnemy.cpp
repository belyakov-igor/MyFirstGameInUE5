#include "AI/Services/AMyServicePickEnemy.h"

#include "AI/Components/MyAIPerceptionComponent.h"
#include "Controllers/AIControllerBase.h"
#include "Characters/AICharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UAMyServicePickEnemy::UAMyServicePickEnemy()
{
	bCreateNodeInstance = false;
	bNotifyBecomeRelevant = true;
	NodeName = "Find Enemy";
}

void UAMyServicePickEnemy::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	const auto Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)
	{
		return;
	}
	AskAlliesIfThereIsAnEnemy(Controller);
}

void UAMyServicePickEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return;
	}

	const auto Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	const auto PerceptionComponent = Controller->FindComponentByClass<UMyAIPerceptionComponent>();
	if (PerceptionComponent == nullptr)
	{
		return;
	}

	auto Enemy = PerceptionComponent->GetEnemy();
	auto BBEnemy = Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName));
	if (Cast<ACharacterBase>(BBEnemy) != nullptr && Cast<ACharacterBase>(BBEnemy)->IsDead())
	{
		BBEnemy = nullptr;
	}
	if (Enemy == nullptr)
	{
		Enemy = BBEnemy;
		if (Enemy != nullptr)
		{
			auto Pawn = Controller->GetPawn();
			if (Pawn != nullptr)
			{
				auto Distance = (Pawn->GetActorLocation() - Enemy->GetActorLocation()).Length();
				if (DistanceToLoseNotPerceivedEnemy < Distance)
				{
					Enemy = nullptr;
				}
			}
		}
	}
	Blackboard->SetValueAsObject(Controller->EnemyActorKeyName, Enemy);

	if (BBEnemy == nullptr && Enemy != nullptr /*New enemy found?*/)
	{
		TellAlliesThatThereIsAnEnemy(Controller, Enemy);
	}
}

void UAMyServicePickEnemy::TellAlliesThatThereIsAnEnemy(AAIControllerBase* Controller, AActor* Enemy) const
{
	auto Pawn = Controller->GetPawn();
	if (Pawn == nullptr || Enemy == nullptr)
	{
		check(false);
		return;
	}
	auto MaxDistanceSquared = Controller->MaxDistanceToAllyToTellItAboutEnemy;
	MaxDistanceSquared *= MaxDistanceSquared;
	auto Location = Pawn->GetActorLocation();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIControllerBase::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		if (Actor == Controller)
		{
			continue;
		}
		auto AIController = Cast<AAIControllerBase>(Actor);
		check(AIController != nullptr);
		if (AIController->GroupId != Controller->GroupId)
		{
			continue;
		}
		auto AICharacter = Cast<ACharacterBase>(AIController->GetPawn());
		if (
			AICharacter != nullptr
			&& !AICharacter->IsDead()
			&& (AICharacter->GetActorLocation() - Location).SquaredLength() < MaxDistanceSquared
		)
		{
			auto AllyController = Cast<AAIControllerBase>(AICharacter->GetController());
			auto AllyBlackboard = AllyController->GetBrainComponent()->GetBlackboardComponent();
			if (AllyBlackboard->GetValueAsObject(AllyController->EnemyActorKeyName) == nullptr)
			{
				AllyBlackboard->SetValueAsObject(AllyController->EnemyActorKeyName, Enemy);
			}
		}
	}
}

void UAMyServicePickEnemy::AskAlliesIfThereIsAnEnemy(AAIControllerBase* Controller) const
{
	auto Pawn = Controller->GetPawn();
	const auto Blackboard = Controller->GetBrainComponent()->GetBlackboardComponent();
	if (Pawn == nullptr || Blackboard == nullptr)
	{
		check(false);
		return;
	}
	auto MaxDistanceSquared = Controller->MaxDistanceToAllyToTellItAboutEnemy;
	MaxDistanceSquared *= MaxDistanceSquared;
	auto Location = Pawn->GetActorLocation();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIControllerBase::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		auto AIController = Cast<AAIControllerBase>(Actor);
		check(AIController != nullptr);
		if (AIController->GroupId != Controller->GroupId)
		{
			continue;
		}
		auto AICharacter = AIController->GetPawn();
		if (
			AICharacter != nullptr
			&& (AICharacter->GetActorLocation() - Location).SquaredLength() < MaxDistanceSquared
		)
		{
			auto AllyController = Cast<AAIControllerBase>(AICharacter->GetController());
			auto AllyBlackboard = AllyController->GetBrainComponent()->GetBlackboardComponent();
			if (auto Enemy = AllyBlackboard->GetValueAsObject(AllyController->EnemyActorKeyName); Enemy != nullptr)
			{
				Blackboard->SetValueAsObject(Controller->EnemyActorKeyName, Enemy);
				return;
			}
		}
	}
}
