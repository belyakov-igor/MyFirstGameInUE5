#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TriggerOnBeginPlayComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UTriggerOnBeginPlayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTriggerOnBeginPlayComponent() { PrimaryComponentTick.bCanEverTick = false; }

protected:
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle;
	void Trigger();
};
