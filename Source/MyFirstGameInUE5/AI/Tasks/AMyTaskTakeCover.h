#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"

#include "AMyTaskTakeCover.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UAMyTaskTakeCover : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UAMyTaskTakeCover();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Cover", meta = (ClampMin = 0.2f))
	float TimeInCover = 4.f;

private:
	class AAIControllerBase* Controller;
	FTimerHandle TimerHandle;
	class APlaceToCover* Cover = nullptr;

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void OnDamageTaken(FName BoneName, float Damage);

	class APlaceToCover* FindCover(UBehaviorTreeComponent& OwnerComp) const;

	void TimeInCoverExpired(bool Succeeded);
	void TimeInCoverExpiredWithSuccess();
	void RemoveDelegateFromDamageTakerComponent();
};
