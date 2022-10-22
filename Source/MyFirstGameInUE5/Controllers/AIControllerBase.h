#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Global/MySaveGame.h"

#include "BehaviorTree/BehaviorTreeTypes.h"

#include "AIControllerBase.generated.h"

USTRUCT()
struct FMoveTargetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame)
	FVector Position;

	UPROPERTY(SaveGame)
	FRotator Rotation;

	UPROPERTY(SaveGame)
	float TimeToStayInPosition;
};

USTRUCT()
struct FMoveTargetSequenceTaskData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame)
	TArray<FMoveTargetData> OrderMoveTagetSequence;

	UPROPERTY(SaveGame)
	int32 FirstNonMandatoryOrderMoveTarget = 0;

	UPROPERTY(SaveGame)
	bool bLoopTargetSequence = false;

	UPROPERTY(SaveGame)
	uint16 CurrentMoveTargetIndex = 0;

	UPROPERTY(SaveGame)
	bool bCurrentLoopIsFirst = true;
};

USTRUCT()
struct FSavedBlackboardKeys
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(SaveGame)
	FName EnemyActorName = NAME_None;

	UPROPERTY(SaveGame)
	bool bMoveTargetIsMandatory = true;

	UPROPERTY(SaveGame)
	FVector LastDamageTakenDirection = FVector(1.f, 0.f, 0.f);

	UPROPERTY(SaveGame)
	bool bDamageIsTakenRecently = false;

	UPROPERTY(SaveGame)
	float DamageIsTakenRecentlyTime = -1.f; // negative means that timer shouldn't be started

	UPROPERTY(SaveGame)
	bool bFightHasStarted = false;

	UPROPERTY(SaveGame)
	bool bGotShotByUnknownEnemy = false;
};

UCLASS()
class MYFIRSTGAMEINUE5_API AAIControllerBase : public AAIController, public ISavable
{
	GENERATED_BODY()

public:
	explicit AAIControllerBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName EnemyActorKeyName = "EnemyActor";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName MoveTargetIsMandatoryKeyName = "MoveTargetIsMandatory";

	/// point to the direction where the damage came from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName LastDamageTakenDirectionKeyName = "LastDamageTakenDirection";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName DamageIsTakenRecentlyKeyName = "DamageIsTakenRecently";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName FightHasStartedKeyName = "FightHasStarted";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName GotShotByUnknownEnemyKeyName = "GotShotByUnknownEnemy";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TimeToRememberTakenDamage = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RadiusOfCoverSearch = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MinDistanceFromCoverToEnemy = 500.f;

	void SetOrderMoveTagetSequence(FMoveTargetSequenceTaskData MoveTargetSequenceTaskData);
	FMoveTargetSequenceTaskData& GetMoveTargetSequenceTaskData() { return MoveTargetSequenceTaskData; }

	// Properties for game saving
	UPROPERTY(SaveGame)
	FName SavedPossessedCharacterName = NAME_None;

	virtual TArray<uint8> GetActorSaveData() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UMyAIPerceptionComponent* MyAIPerceptionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset = nullptr;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

private:
	UPROPERTY(SaveGame)
	FMoveTargetSequenceTaskData MoveTargetSequenceTaskData;

	UPROPERTY(SaveGame)
	FSavedBlackboardKeys SavedBlackboardKeys;

	void InitBlackboardAndBehaviorTree();
	void SubscribeToDamageTakerMomentumDelegate();
	UFUNCTION()
	void OnMomentumTaken(FName BoneName, FVector ImpactPoint, FVector Momentum);
	FTimerHandle MomentumTakenTimerHandle;
	void MomentumTakenIsNotRecentAnymore();
};
