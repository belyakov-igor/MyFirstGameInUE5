#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "AMyServicePickEnemy.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyServicePickEnemy : public UBTService
{
	GENERATED_BODY()

public:
	explicit UAMyServicePickEnemy();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/// <summary>
	/// If perception component no longer sees the enemy, it will not remove it from blackboard until it is
	/// this far away.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DistanceToLoseNotPerceivedEnemy = 3500.f;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void TellAlliesThatThereIsAnEnemy(class AAIControllerBase* Controller, AActor* Enemy) const;
	void AskAlliesIfThereIsAnEnemy(class AAIControllerBase* Controller) const;
};
