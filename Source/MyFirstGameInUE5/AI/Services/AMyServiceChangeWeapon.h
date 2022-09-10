#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "AMyServiceChangeWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyServiceChangeWeapon : public UBTService
{
	GENERATED_BODY()

public:
	explicit UAMyServiceChangeWeapon();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
