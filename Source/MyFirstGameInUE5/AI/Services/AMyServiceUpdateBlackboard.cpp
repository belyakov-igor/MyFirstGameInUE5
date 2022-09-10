#include "AI/Services/AMyServiceUpdateBlackboard.h"

#include "Controllers/AIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UAMyServiceUpdateBlackboard::UAMyServiceUpdateBlackboard()
{
	NodeName = "Update blackboard";
}

void UAMyServiceUpdateBlackboard::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return;
	}

	const auto Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	auto EnemyActor = Blackboard->GetValueAsObject(Controller->EnemyActorKeyName);
	auto MoveTargetIsMandatory = Blackboard->GetValueAsBool(Controller->MoveTargetIsMandatoryKeyName);
	auto DamageIsTakenRecently = Blackboard->GetValueAsBool(Controller->DamageIsTakenRecentlyKeyName);

	Blackboard->SetValueAsBool(
		Controller->FightHasStartedKeyName
		,
			!MoveTargetIsMandatory
			&&
			(
				EnemyActor != nullptr
				|| DamageIsTakenRecently
			)
	);
}
