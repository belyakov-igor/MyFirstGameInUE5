#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "TriggerOnBoxOverlapComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UTriggerOnBoxOverlapComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UTriggerOnBoxOverlapComponent() { PrimaryComponentTick.bCanEverTick = false; SetGenerateOverlapEvents(true); }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);
};
