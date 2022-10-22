#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "AMyTaskFaceDirection.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyTaskFaceDirection : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UAMyTaskFaceDirection();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Cover", meta = (ClampMin = 0.2f))
	float TimeInThisTask = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FBlackboardKeySelector DirectionKey;

private:
	class AAIControllerBase* Controller;
	FTimerHandle TimerHandle;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	void Succeed();
};
