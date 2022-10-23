#include "AI/Services/AMyServicePickEnemy.h"

#include "AI/Components/MyAIPerceptionComponent.h"
#include "Controllers/AIControllerBase.h"
#include "Characters/AICharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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
	auto BBEnemy = Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName));
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

	// Tell allies that there is an enemy
	if (BBEnemy == nullptr && Enemy != nullptr /*New enemy found?*/)
	{
		auto Pawn = Controller->GetPawn();
		if (ensure(Pawn != nullptr))
		{
			auto MaxDistanceSquared = Controller->MaxDistanceToAllyToTellItAboutEnemy;
			MaxDistanceSquared *= MaxDistanceSquared;
			auto Location = Pawn->GetActorLocation();

			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAICharacter::StaticClass(), Actors);
			for (auto Actor : Actors)
			{
				auto AICharacter = Cast<AAICharacter>(Actor);
				if (
					AICharacter != nullptr
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
	}
}
