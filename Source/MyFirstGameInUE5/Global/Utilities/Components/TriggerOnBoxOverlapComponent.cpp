#include "TriggerOnBoxOverlapComponent.h"

#include "Global/Utilities/MyUtilities.h"
#include "Characters/PlayerCharacter.h"

void UTriggerOnBoxOverlapComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UTriggerOnBoxOverlapComponent::OnBeginOverlap);
}

void UTriggerOnBoxOverlapComponent::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent
	, AActor* OtherActor
	, UPrimitiveComponent* OtherComp
	, int32 OtherBodyIndex
	, bool bFromSweep
	, const FHitResult& SweepResult
)
{
	if (Cast<APlayerCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	auto Triggerable = Cast<ITriggerable>(GetOwner());

	if (Triggerable == nullptr)
	{
		checkf(false, TEXT("Use this component class only with actors implementing ITriggerable interface"));
		return;
	}

	if (ITriggerable::Execute_CanBeTriggered(GetOwner()))
	{
		ITriggerable::Execute_Trigger(GetOwner());
	}
}
