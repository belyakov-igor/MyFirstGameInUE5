#pragma once

#include "Interaction/InteractableActor.h"
#include "Global/MySaveGame.h"

#include "CoreMinimal.h"

#include "WeaponPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AWeaponPickup : public AInteractableActor, public ISavable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	TSubclassOf<class ABaseWeapon> WeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText WeaponName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = 0))
	int32 AmmoAmount = 10;

protected:
	virtual void Interact_Implementation(class APawn* Pawn) override;
	virtual FText FocusText_Implementation() override;
};
