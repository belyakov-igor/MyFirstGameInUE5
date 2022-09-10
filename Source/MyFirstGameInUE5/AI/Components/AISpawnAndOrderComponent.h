#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "AISpawnAndOrderComponent.generated.h"


USTRUCT(BlueprintType)
struct FWeaponAndAmmo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class ABaseWeapon> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = 0))
	int32 AmmoAmount = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UAISpawnAndOrderComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UAISpawnAndOrderComponent();

	void ScheduleSpawn();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FWeaponAndAmmo> WeaponAndAmmoArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = 0.f))
	float SpawnDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = 0.f))
	int32 FirstNonMandatoryOrderMoveTarget = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = 0.f))
	bool LoopTargetSequence = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = 0.f))
	TSubclassOf<class AAICharacter> CharacterClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = 0.f))
	TSubclassOf<class AAIControllerBase> ControllerClass = nullptr;

private:
	FTimerHandle TimerHandle;
};
