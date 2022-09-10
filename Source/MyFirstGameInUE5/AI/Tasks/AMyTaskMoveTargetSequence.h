#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"

#include "AMyTaskMoveTargetSequence.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyTaskMoveTargetSequence : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UAMyTaskMoveTargetSequence();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	uint16 MoveTargetIndex = 0;
	bool bFirstLoop = true;
	class AAIControllerBase* Controller;
	FTimerHandle TimerHandle;

	EBTNodeResult::Type Move(UBehaviorTreeComponent& OwnerComp);

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	void UpdateBlackBoardKeyTargetIsMandatory();
};
