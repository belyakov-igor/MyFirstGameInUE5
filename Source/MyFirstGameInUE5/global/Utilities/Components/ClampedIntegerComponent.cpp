#include "ClampedIntegerComponent.h"

UClampedIntegerComponent::UClampedIntegerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UClampedIntegerComponent::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(Min <= Max, TEXT("Min should be lesser than Max (%d < %d)"), Max, Min);

	SetValue(FMath::Clamp(Value, Min, Max));
}

void UClampedIntegerComponent::SetValue(int32 NewValue)
{
	Value = FMath::Clamp(NewValue, Min, Max);
	ValueChanged.Broadcast(Value);

	if (Value == Min)
	{
		ReachedMin.Broadcast();
	}
	else if (Value == Max)
	{
		ReachedMax.Broadcast();
	}
}

int32 UClampedIntegerComponent::GetValue() const
{
	return Value;
}

void UClampedIntegerComponent::UClampedIntegerComponent::Increase(int32 Delta)
{
	checkf(Delta >= 0, TEXT("Delta must be non-negative."))
	if (Delta > 0 && Value != Max)
	{
		SetValue(Value + Delta);
	}
}

void UClampedIntegerComponent::Decrease(int32 Delta)
{
	checkf(Delta >= 0, TEXT("Delta must be non-negative."))
	if (Delta > 0 && Value != Min)
	{
		SetValue(Value - Delta);
	}
}
