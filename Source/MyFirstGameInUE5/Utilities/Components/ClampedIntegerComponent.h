#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ClampedIntegerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FClampedIntegerComponent_ValueChanged_Signature, int32);
DECLARE_MULTICAST_DELEGATE(FClampedIntegerComponent_ReachedMin_Signature);
DECLARE_MULTICAST_DELEGATE(FClampedIntegerComponent_ReachedMax_Signature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UClampedIntegerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UClampedIntegerComponent();

	FClampedIntegerComponent_ValueChanged_Signature ValueChanged;
	FClampedIntegerComponent_ReachedMin_Signature ReachedMin;
	FClampedIntegerComponent_ReachedMax_Signature ReachedMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	int32 Min = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	int32 Max = 100;


	UFUNCTION(BlueprintCallable, Category = "Value")
	void SetValue(int32 NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Value")
	int32 GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Increase(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Value")
	void Decrease(int32 Delta);

protected:
	virtual void BeginPlay() override;

private:
	int32 Value = 0;
};
