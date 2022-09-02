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
		check(InteractableActor != nullptr);
		float dot_ = FVector::DotProduct((InteractableActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal(), GetForwardVector());
		if (dot_ > dot)
		{
			dot = dot_;
			ActorInFocus = InteractableActor;
		}
	}

	if (ActorInFocus != nullptr)
	{
		ChangeHUDText.Broadcast(ActorInFocus->FocusText());
	}
	else
	{
		ChangeHUDText.Broadcast(FText::GetEmpty());
	}

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
