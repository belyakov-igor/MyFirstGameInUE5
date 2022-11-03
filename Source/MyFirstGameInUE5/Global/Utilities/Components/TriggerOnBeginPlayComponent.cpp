#include "TriggerOnBeginPlayComponent.h"

#include "Global/Utilities/MyUtilities.h"

void UTriggerOnBeginPlayComponent::BeginPlay()
{
	Super::BeginPlay();

	// need to postpone trigger so that triggerable's BeginPlay would be executed before it is triggered
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTriggerOnBeginPlayComponent::Trigger, 0.1f);
}

void UTriggerOnBeginPlayComponent::Trigger()
{
	auto Triggerable = Cast<ITriggerable>(GetOwner());

	if (Triggerable == nullptr)
	{
		checkf(false, TEXT("Use this class only with objects implementing ITriggerable interface"));
		return;
	}

	if (ITriggerable::Execute_CanBeTriggered(GetOwner()))
	{
		ITriggerable::Execute_Trigger(GetOwner());
	}
}
