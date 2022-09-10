#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "AMyTaskSetBoolBlackboardValue.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyTaskSetBoolBlackboardValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UAMyTaskSetBoolBlackboardValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FBlackboardKeySelector BoolValueKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool Value = false;

private:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
};
