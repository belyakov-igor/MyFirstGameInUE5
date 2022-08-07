#pragma once

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

	/** Weapons that the owner character currently has. Supposed to
	 *  be added during gameplay as well as in the BeginPlay()
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<ABaseWeapon*> Weapons;

	/** What weapons should be spawned and added to Weapons array in the BeginPlay() */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> DefaultWeapons;


	/** Return true if succeeded*/
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool SetCurrentWeapon(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetNextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetPreviousWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentWeaponIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ABaseWeapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	void BeginAttack();
	void EndAttack();

	void BeginAim();
	void EndAim();

	void AddWeapon(ABaseWeapon* Weapon);

	bool AttackIsBeingPerformed() const;

	FSignalSignature OnAttackFinished;

	bool HasValidWeapon() const;

protected:
	virtual void BeginPlay() override;

private:
	ABaseWeapon* CurrentWeapon() const;
	void SpawnAndAddDefaultWeapons();

	int32 CurrentWeaponIndex = 0;
};
