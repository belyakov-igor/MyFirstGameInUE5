#pragma once

#include "Global/Utilities/MyUtilities.h"
#include "Weapons/WeaponUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"

#include "BaseWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName HandGripSocketName = "HandGripSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float MaxShotDistance = 10000.f;

	/** In degrees */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float BulletSpread = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	class UTexture2D* WeaponIcon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	class UTexture2D* CrossHairIcon = nullptr;

	virtual void BeginAttack() {}
	virtual void EndAttack() {}

	virtual void SwitchCharacterToAnimationSet() const { checkNoEntry(); };

	bool AttackIsBeingPerformed() const { return bAttackIsBeingPerformed; }

	void AttachToOwner(class ACharacter* NewOwner);

	struct FWeaponUIData GetUIData() const;

	FSignalSignature OnAttackFinished;

protected:
	bool bAttackIsBeingPerformed = false;

	APlayerController* GetPlayerController() const;

	USkeletalMeshComponent* GetCharacterMesh() const;
	std::pair<ACharacter*, USkeletalMeshComponent*> GetCharacterAndCharacterMesh() const;

	FHitResult MakeTrace() const;
};
