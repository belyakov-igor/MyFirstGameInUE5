#include "AI/Actors/AISpawnAndOrderActor.h"

#include "AI/Components/AISpawnAndOrderComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AAISpawnAndOrderActor::AAISpawnAndOrderActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnTriggerComponent = CreateDefaultSubobject<UBoxComponent>("SpawnTriggerComponent");
	SetRootComponent(SpawnTriggerComponent);
	SpawnTriggerComponent->InitBoxExtent(FVector(100.f));
}

void AAISpawnAndOrderActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bAlreadyTriggered)
	{
		return;
	}
	bAlreadyTriggered = true;
	{
		auto Pawn = Cast<APawn>(OtherActor);
		if (Pawn == nullptr || !Pawn->IsPlayerControlled())
		{
			return;
		}
	}

	TArray<UActorComponent*> SOComponents;
	GetComponents(UAISpawnAndOrderComponent::StaticClass(), SOComponents);
	for (auto Component : SOComponents)
	{
		auto SOComponent = Cast<UAISpawnAndOrderComponent>(Component);
		if (ensure(SOComponent))
		{
			SOComponent->ScheduleSpawn();
		}
	}
}
