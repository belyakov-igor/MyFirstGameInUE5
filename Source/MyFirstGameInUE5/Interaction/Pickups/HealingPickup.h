#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActor.h"

#include "HealingPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AHealingPickup : public AInteractableActor
{
	GENERATED_BODY()

public:
	virtual FText FocusText() override;
	virtual void Interact(class APawn* Pawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 30;
};
