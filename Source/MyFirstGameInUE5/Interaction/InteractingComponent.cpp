#include "Interaction/InteractingComponent.h"

#include "Interaction/InteractableActor.h"

UInteractingComponent::UInteractingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractingComponent::AddInteractableActor(AInteractableActor* InteractableActor)
{
	if (InteractableActors.Find(InteractableActor) != INDEX_NONE)
	{
		return;
	}
	InteractableActors.Add(InteractableActor);
}

void UInteractingComponent::RemoveInteractableActor(AInteractableActor* InteractableActor)
{
	if (InteractableActors.Find(InteractableActor) == INDEX_NONE)
	{
		return;
	}
	InteractableActors.Remove(InteractableActor);
}

void UInteractingComponent::Interact()
{
	if (ActorInFocus != nullptr)
	{
		check(ActorInFocus->bIsEnabled);
		ActorInFocus->Interact(Cast<APawn>(GetOwner()));
	}
}

void UInteractingComponent::TickComponent(float DeltaTime, enum ELevelTick /*TickType*/, FActorComponentTickFunction* /*ThisTickFunction*/)
{
	check(GetOwner() != nullptr);
	float dot = MinDotProductForInteractableDirection;
	auto OldActorInFocus = ActorInFocus;
	ActorInFocus = nullptr;
	for (auto InteractableActor : InteractableActors)
	{
		if (!InteractableActor->bIsEnabled)
		{
			continue;
		}
		check(InteractableActor != nullptr);
		float dot_ = FVector::DotProduct((InteractableActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal(), GetForwardVector());
		if (dot_ > dot)
		{
			dot = dot_;
			ActorInFocus = InteractableActor;
		}
	}

	ChangeHUDText.Broadcast(ActorInFocus != nullptr ? ActorInFocus->FocusText() : FText::GetEmpty());

	if (OldActorInFocus != ActorInFocus)
	{
		if (OldActorInFocus != nullptr)
		{
			OldActorInFocus->FocusOut.Broadcast();
		}
		if (ActorInFocus != nullptr)
		{
			ActorInFocus->FocusIn.Broadcast();
		}
	}
}

void UInteractingComponent::BeginPlay()
{
	Super::BeginPlay();
}
