#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "AMyTaskFire.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyTaskFire : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UAMyTaskFire();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float MaxTimeForThisTask = 4.f;

	/** Character will not fire until this time expired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float AimingTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 MaxShots = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float TraceInterval = 1.f;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	virtual uint16 GetInstanceMemorySize() const override;

	void InitMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	EBTNodeResult::Type Fire(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void MakeTraceIfNeeded(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	struct CharacterData
	{
		class AAIControllerBase* Controller;
		class APawn* Pawn;
		class UWeaponManagerComponent* WeaponManagerComponent;
		class UAmmoComponent* AmmoComponent;
		class AActor* Target;
	};
	CharacterData GetCharacterData(UBehaviorTreeComponent& OwnerComp);

	struct TNodeMemory
	{
		float InitialTime = 0.f;
		uint16 InitialAmmo = 0;
		uint16 TracesDone = 0;
		bool TraceSucceeded = false;
	};

	EBTNodeResult::Type GetStatus(const CharacterData& Data, TNodeMemory* Memory);
	void Cleanup(UBehaviorTreeComponent& OwnerComp);
};
