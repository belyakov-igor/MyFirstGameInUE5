#pragma once

#include "Interaction/InteractableActor.h"
#include "Global/MySaveGame.h"

#include "CoreMinimal.h"

#include "HealingPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AHealingPickup : public AInteractableActor, public ISavable
{
	GENERATED_BODY()

public:
	virtual FText FocusText() override;
	virtual void Interact(class APawn* Pawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 30;
};
