#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActor.h"

#include "WeaponPickup.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AWeaponPickup : public AInteractableActor
{
	GENERATED_BODY()

public:
	virtual FText FocusText() override;
	virtual void Interact(class APawn* Pawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	TSubclassOf<class ABaseWeapon> WeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText WeaponName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = 0))
	int32 AmmoAmount = 10;
};
