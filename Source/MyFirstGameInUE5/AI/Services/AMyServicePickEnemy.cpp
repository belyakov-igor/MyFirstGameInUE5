#include "AI/Services/AMyServicePickEnemy.h"

#include "AI/Components/MyAIPerceptionComponent.h"
#include "Controllers/AIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UAMyServicePickEnemy::UAMyServicePickEnemy()
{
	NodeName = "Find Enemy";
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
	if (Enemy == nullptr)
	{
		Enemy = Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName));
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
}
