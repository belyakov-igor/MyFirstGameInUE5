#include "Interaction/InteractableActor.h"

#include "Interaction/InteractingComponent.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetSimulatePhysics(true);
	SetRootComponent(Mesh);

	PawnOverlapCollisionComponent = CreateDefaultSubobject<USphereComponent>("PawnOverlapCollisionComponent");
	PawnOverlapCollisionComponent->InitSphereRadius(100.f);
	PawnOverlapCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	PawnOverlapCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PawnOverlapCollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	PawnOverlapCollisionComponent->SetupAttachment(Mesh);
}

void AInteractableActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (auto InteractingComponent = FindInteractingComponent(OtherActor); InteractingComponent != nullptr)
	{
		InteractingComponent->AddInteractableActor(this);
	}
}

void AInteractableActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (auto InteractingComponent = FindInteractingComponent(OtherActor); InteractingComponent != nullptr)
	{
		InteractingComponent->RemoveInteractableActor(this);
	}
}

UInteractingComponent* AInteractableActor::FindInteractingComponent(AActor* Actor)
{
	return Actor == nullptr ? nullptr : Cast<UInteractingComponent>(Actor->FindComponentByClass(UInteractingComponent::StaticClass()));
}
