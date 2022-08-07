#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ClampedIntegerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UClampedIntegerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UClampedIntegerComponent();

	FInt32Int32MulticastSignature ValueChanged;
	FSignalMulticastSignature ReachedMin;
	FSignalMulticastSignature ReachedMax;

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
