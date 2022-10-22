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

	virtual ECharacterAnimationSet GetCharacterAnimationSet() const override
    { return ECharacterAnimationSet::Pistol; }

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* CrouchFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* UprightFireAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    class UAmmoComponent* AmmoComponent;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    class UStandardFirearmFXComponent* StandardFirearmFXComponent;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float TimeBetweenShots = 0.3f;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float Damage = 25.f;

    UPROPERTY(EditDefaultsOnly, Category = "General")
    float BulletMomentum = 400.f;

    virtual float GetDPS() const override { return Damage / TimeBetweenShots; }

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle FireTimerHandle;
    void PistolShotEnded();
};
