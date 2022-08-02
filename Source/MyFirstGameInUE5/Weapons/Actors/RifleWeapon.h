#pragma once

#include "CoreMinimal.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "RifleWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ARifleWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
    ARifleWeapon();

	virtual void BeginAttack() override;
	virtual void EndAttack() override;

	virtual void SwitchCharacterToAnimationSet() const override;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* CrouchFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* UprightFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    class UAmmoComponent* AmmoComponent;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float TimeBetweenShots = 0.15f;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float Damage = 10.f;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle FireTimerHandle;
};
