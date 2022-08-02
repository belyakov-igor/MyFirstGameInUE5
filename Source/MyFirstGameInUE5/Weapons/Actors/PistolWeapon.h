#pragma once

#include "CoreMinimal.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "PistolWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APistolWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
    APistolWeapon();

    virtual void BeginAttack() override;

	virtual void SwitchCharacterToAnimationSet() const override;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* CrouchFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* UprightFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    class UAmmoComponent* AmmoComponent;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float TimeBetweenShots = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float Damage = 5.f;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle FireTimerHandle;
};
