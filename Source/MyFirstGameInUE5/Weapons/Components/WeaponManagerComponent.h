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

	virtual void BeginAttack();
	virtual void EndAttack();

	void AddWeapon(ABaseWeapon* Weapon);

	bool AttackIsBeingPerformed() const;
	bool HasValidWeapon() const;

	FOnAttackFinished OnAttackFinished;

protected:
	virtual void BeginPlay() override;

	// Weapons that the owner character currently has. Supposed to
	// be added during gameplay as well as in the BeginPlay()
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<ABaseWeapon*> Weapons;

	// What weapons should be spawned and added to Weapons array in the BeginPlay()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> DefaultWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = 0))
	int32 CurrentWeaponIndex = 0;

private:
	ABaseWeapon* CurrentWeapon() const;

	void SpawnAndAddDefaultWeapons();
};
