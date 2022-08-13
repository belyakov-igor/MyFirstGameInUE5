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
	NewValue = FMath::Clamp(NewValue, Min, Max);
	if (Value == NewValue)
	{
		return;
	}
	auto OldValue = Value;
	Value = NewValue;
	ValueChanged.Broadcast(OldValue, Value);

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
	if (Delta > 0)
	{
		SetValue(Value + Delta);
	}
}

void UClampedIntegerComponent::Decrease(int32 Delta)
{
	checkf(Delta >= 0, TEXT("Delta must be non-negative."))
	if (Delta > 0)
	{
		SetValue(Value - Delta);
	}
}
