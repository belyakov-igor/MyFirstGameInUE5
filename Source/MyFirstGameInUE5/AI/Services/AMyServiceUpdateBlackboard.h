#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "AMyServiceUpdateBlackboard.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyServiceUpdateBlackboard : public UBTService
{
	GENERATED_BODY()

public:
	explicit UAMyServiceUpdateBlackboard();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
