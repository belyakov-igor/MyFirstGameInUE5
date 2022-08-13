#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActor.h"

#include "AmmoPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAmmoPickup : public AInteractableActor
{
	GENERATED_BODY()

public:
	virtual FText FocusText() override;
	virtual void Interact(class APawn* Pawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText WeaponName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	TSubclassOf<class ABaseWeapon> WeaponClass = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 10;
};
