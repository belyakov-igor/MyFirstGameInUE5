#include "AI/Tasks/AMyTaskSetBoolBlackboardValue.h"

#include "Controllers/AIControllerBase.h"
#include "Characters/AICharacter.h"
#include "AI/Actors/PlaceToCover.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UAMyTaskSetBoolBlackboardValue::UAMyTaskSetBoolBlackboardValue()
{
	bCreateNodeInstance = false;
	bNotifyTick = false;
	NodeName = "Set bool blackboard value";

	BoolValueKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UAMyTaskSetBoolBlackboardValue, BoolValueKey));
}

void UAMyTaskSetBoolBlackboardValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BoolValueKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UAMyTaskSetBoolBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsBool(BoolValueKey.SelectedKeyName, Value);

	return EBTNodeResult::Succeeded;
}
