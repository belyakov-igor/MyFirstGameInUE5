#pragma once

#include "Global/Utilities/MyUtilities.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WeaponManagerComponent.generated.h"

class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponManagerComponent();

	static constexpr int32 MinWeaponSlot = 0;
	static constexpr int32 MaxWeaponSlot = 9;

	/** 10 slots with weapons that the owner character currently has.
	 *  Empty slots contain nullptr.
	 *  Weapons are supposed to be added to their slots during gameplay.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<ABaseWeapon*> Weapons;

	/** Return true if succeeded*/
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetCurrentWeaponSlot(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetNextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetPreviousWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentWeaponSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ABaseWeapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	void BeginAttack();
	void EndAttack();

	void SetWeaponVisibility(bool Visible);

	void AddWeapon(ABaseWeapon* Weapon);

	bool AttackIsBeingPerformed() const;

	void SetIsCrouching(bool IsCrouching);

	FSignalSignature OnAttackFinished;
	FSignalSignature OnWeaponAndAmmoChanged;

	TArray<uint8> GetSerializedData() const;
	void ApplySerializedData(const TArray<uint8>& Data);

protected:
	virtual void BeginPlay() override;

private:
	int32 CurrentWeaponSlot = 1;
	bool bWeaponIsVisible = false;
};
