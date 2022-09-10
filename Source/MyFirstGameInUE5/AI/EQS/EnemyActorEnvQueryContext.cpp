#include "AI/EQS/EnemyActorEnvQueryContext.h"

#include "Controllers/AIControllerBase.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEnemyActorEnvQueryContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	const auto QueryOwner = Cast<APawn>(QueryInstance.Owner.Get());
	if (QueryOwner == nullptr)
	{
		return;
	}
	auto Controller = Cast<AAIControllerBase>(QueryOwner->GetController());
	if (Controller == nullptr)
	{
		return;
	}
	auto Blackboard = Controller->GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return;
	}
	auto Actor = Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName));
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Actor);
}
