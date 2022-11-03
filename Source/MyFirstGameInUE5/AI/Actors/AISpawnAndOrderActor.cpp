#include "AI/Actors/AISpawnAndOrderActor.h"

#include "AI/Components/AISpawnAndOrderComponent.h"
#include "AI/Components/AIOrderMoveTargetComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AAISpawnAndOrderActor::AAISpawnAndOrderActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);

	OrderMoveTargetsRoot = CreateDefaultSubobject<USceneComponent>("OrderMoveTargetsRoot");
	OrderMoveTargetsRoot->SetupAttachment(RootScene);
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
	if (bAlreadyTriggered)
	{
		return;
	}
	bAlreadyTriggered = true;
	Triggered.Broadcast();
	Spawn();
}

const TMap<FName, const class UAIOrderMoveTargetComponent*>& AAISpawnAndOrderActor::GetOrderMoveTargets()
{
	if (!OrderMoveTargetsIsValid)
	{
		OrderMoveTargets = MakeOrderMoveTargetsMap();
		OrderMoveTargetsIsValid = true;
	}
	return OrderMoveTargets;
}

void AAISpawnAndOrderActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAlreadyTriggered)
	{
		Spawn();
	}
}

void AAISpawnAndOrderActor::Spawn()
{
	TArray<UActorComponent*> SOComponents;
	GetComponents(UAISpawnAndOrderComponent::StaticClass(), SOComponents);
	bool SaveDataAvailable = SavedSpawnDelays.Num() != 0;
	if (SaveDataAvailable)
	{
		check(SavedSpawnDelays.Num() == SOComponents.Num());
	}
	for (int32 i = 0; i != SOComponents.Num(); ++i)
	{
		auto SOComponent = Cast<UAISpawnAndOrderComponent>(SOComponents[i]);
		if (ensure(SOComponent))
		{
			if (SaveDataAvailable)
			{
				SOComponent->SetSpawnDelay(SavedSpawnDelays[i]);
			}
			SOComponent->ScheduleSpawn();
		}
	}
}

TMap<FName, const class UAIOrderMoveTargetComponent*> AAISpawnAndOrderActor::MakeOrderMoveTargetsMap() const
{
	TArray<USceneComponent*> OrderMoveTargets_;
	check(OrderMoveTargetsRoot != nullptr);
	OrderMoveTargetsRoot->GetChildrenComponents(/*bIncludeAllDescendants*/ true, OrderMoveTargets_);

	TMap<FName, const class UAIOrderMoveTargetComponent*> Ret;
	Ret.Reserve(OrderMoveTargets_.Num());
	for (auto Child : OrderMoveTargets_)
	{
		auto Component = Ret.Emplace(Child->GetFName(), Cast<UAIOrderMoveTargetComponent>(Child));
		check(Component != nullptr);
	}

	return Ret;
}
