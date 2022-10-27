#include "TriggerOnBeginPlayComponent.h"

#include "Global/Utilities/MyUtilities.h"

void UTriggerOnBeginPlayComponent::BeginPlay()
{
	Super::BeginPlay();

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
