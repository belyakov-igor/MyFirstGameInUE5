#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "BehaviorTree/BehaviorTreeTypes.h"

#include "AIControllerBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAIControllerBase : public AAIController
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

	struct MoveTarget
	{
		FVector Position;
		FRotator Rotation;
		float TimeToStayInPosition;
	};

	void SetOrderMoveTagetSequence(TArray<MoveTarget> MoveTargets, int32 FirstNonMandatoryTaget, bool LoopTargetSequence);

	const TArray<MoveTarget>& GetOrderMoveTagetSequence() const { return OrderMoveTagetSequence; }
	int32 GetFirstNonMandatoryOrderMoveTarget() const { return FirstNonMandatoryOrderMoveTarget; }
	bool GetLoopTargetSequence() const { return bLoopTargetSequence; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UMyAIPerceptionComponent* MyAIPerceptionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset = nullptr;

	virtual void OnPossess(APawn* InPawn) override;

private:
	TArray<MoveTarget> OrderMoveTagetSequence;
	bool bLoopTargetSequence = false;
	int32 FirstNonMandatoryOrderMoveTarget = 0;

	void InitBlackboardAndBehaviorTree();
	void SubscribeToDamageTakerMomentumDelegate();
	UFUNCTION()
	void OnMomentumTaken(FName BoneName, FVector ImpactPoint, FVector Momentum);
	FTimerHandle MomentumTakenTimerHandle;
};
