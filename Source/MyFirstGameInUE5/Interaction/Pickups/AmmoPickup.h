#pragma once

#include "Interaction/InteractableActor.h"
#include "Global/MySaveGame.h"

#include "CoreMinimal.h"

#include "AmmoPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAmmoPickup : public AInteractableActor, public ISavable
{
	GENERATED_BODY()

public:
	virtual FText FocusText() override;
	virtual void Interact(class APawn* Pawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Category = "Pickup")
	FText WeaponName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Category = "Pickup")
	TSubclassOf<class ABaseWeapon> WeaponClass = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 10;
};
