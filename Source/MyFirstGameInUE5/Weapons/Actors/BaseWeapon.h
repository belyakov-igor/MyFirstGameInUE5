#pragma once

#include "Global/Utilities/MyUtilities.h"
#include "Global/MySaveGame.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"

#include "BaseWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ABaseWeapon : public AActor, public ISavable
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	/** It it's 2, weapon will be accessed with key "2" on keyboard */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0, ClampMax = 9))
	int32 Slot = 1;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "Animation")
	bool IsCrouching = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundCue* ReloadSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundCue* EquipSound = nullptr;

	virtual void BeginAttack() {}
	virtual void EndAttack() {}

	virtual ECharacterAnimationSet GetCharacterAnimationSet() const
	{ return ECharacterAnimationSet::Unarmed; };

	bool AttackIsBeingPerformed() const { return bAttackIsBeingPerformed; }

	void AttachToOwner(class ACharacter* NewOwner);

	struct FWeaponUIData GetUIData() const;

	FSignalSignature OnAttackFinished;

	virtual float GetDPS() const { checkNoEntry(); return 0.f; }

	FHitResult MakeTrace() const;

	FTransform GetMuzzleSocketTransform() const;

	// Properties for game saving
	UPROPERTY(SaveGame)
	int32 SavedClipAmount = -1; // negative means no ammo

	UPROPERTY(SaveGame)
	int32 SavedArsenalAmount = -1;

	virtual TArray<uint8> GetActorSaveData_Implementation() override;

protected:
	virtual void BeginPlay() override;

	bool bAttackIsBeingPerformed = false;

	APlayerController* GetPlayerController() const;

	USkeletalMeshComponent* GetCharacterMesh() const;
	std::pair<ACharacter*, USkeletalMeshComponent*> GetCharacterAndCharacterMesh() const;
};
