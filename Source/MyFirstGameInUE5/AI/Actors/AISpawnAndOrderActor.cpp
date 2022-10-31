#include "AI/Actors/AISpawnAndOrderActor.h"

#include "AI/Components/AISpawnAndOrderComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AAISpawnAndOrderActor::AAISpawnAndOrderActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);
}

TArray<uint8> AAISpawnAndOrderActor::GetActorSaveData_Implementation()
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
	return ISavable::GetActorSaveData_Implementation();
}

void AAISpawnAndOrderActor::Trigger_Implementation()
{
	Triggered.Broadcast();
	Spawn();
}

void AAISpawnAndOrderActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAlreadyTriggered)
	{
		TArray<UActorComponent*> SOComponents;
		GetComponents(UAISpawnAndOrderComponent::StaticClass(), SOComponents);
		if (SavedSpawnDelays.Num() != SOComponents.Num())
		{
			check(SavedSpawnDelays.Num() == 0); // no save data available
			return;
		}
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

void AAISpawnAndOrderActor::Spawn()
{
	if (bAlreadyTriggered)
	{
		return;
	}
	bAlreadyTriggered = true;

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
