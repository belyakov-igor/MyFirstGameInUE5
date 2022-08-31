#pragma once

#include "CoreMinimal.h"
#include "Weapons/Actors/BaseWeapon.h"

#include <utility>

#include "UnarmedWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AUnarmedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FName AnimMontageSectionName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FName UnarmedHitSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float TraceSphereRadius = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float PunchMomentum = 40000.f;

	virtual void BeginAttack() override;

protected:
	virtual void BeginPlay() override;

private:
	void InitAnimations();
	FVector GetHitLocation() const;
	void OnHitNotify(USkeletalMeshComponent* Mesh);
	void OnAttackFinishedNotify(USkeletalMeshComponent* Mesh);
};
