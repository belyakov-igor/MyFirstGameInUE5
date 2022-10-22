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

TArray<uint8> AAISpawnAndOrderActor::GetActorSaveData()
{
	if (bAlreadyTriggered)
	{
		TArray<UActorComponent*> SOComponents;
		GetComponents(UAISpawnAndOrderComponent::StaticClass(), SOComponents);
		SavedSpawnDelays.Empty();
		SavedSpawnDelays.Reserve(SOComponents.Num());
		for (auto Component : SOComponents)
		{
			auto SOComponent = Cast<UAISpawnAndOrderComponent>(Component);
			if (ensure(SOComponent))
			{
				SavedSpawnDelays.Add(SOComponent->GetSpawnDelayRemaining());
			}
		}
	}
	return ISavable::GetActorSaveData();
}

void AAISpawnAndOrderActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAlreadyTriggered)
	{
		TArray<UActorComponent*> SOComponents;
		GetComponents(UAISpawnAndOrderComponent::StaticClass(), SOComponents);
		check(SavedSpawnDelays.Num() == SOComponents.Num());
		for (int32 i = 0; i != SOComponents.Num(); ++i)
		{
			auto SOComponent = Cast<UAISpawnAndOrderComponent>(SOComponents[i]);
			if (ensure(SOComponent))
			{
				SOComponent->SetSpawnDelay(SavedSpawnDelays[i]);
			}
		}
		Spawn();
	}
}

void AAISpawnAndOrderActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (
		auto Pawn = Cast<APawn>(OtherActor);
		bAlreadyTriggered || Pawn == nullptr || !Pawn->IsPlayerControlled()
	)
	{
		return;
	}
	bAlreadyTriggered = true;

	Spawn();
}

void AAISpawnAndOrderActor::Spawn()
{
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
