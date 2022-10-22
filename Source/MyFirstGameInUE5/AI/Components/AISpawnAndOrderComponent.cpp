#include "AI/Components/AISpawnAndOrderComponent.h"

#include "Characters/AICharacter.h"
#include "Controllers/AIControllerBase.h"
#include "Global/Utilities/MyUtilities.h"
#include "AI/Components/AIOrderMoveTargetComponent.h"

DEFINE_LOG_CATEGORY_STATIC(AISpawnAndOrderComponent, All, All);

UAISpawnAndOrderComponent::UAISpawnAndOrderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAISpawnAndOrderComponent::Spawn()
{
	FActorSpawnParameters ControllerSpawnParameters;
	ControllerSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FActorSpawnParameters CharacterSpawnParameters;
	CharacterSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const auto Controller = GetWorld()->SpawnActor<AAIControllerBase>(ControllerClass, ControllerSpawnParameters);
	if (Controller == nullptr)
	{
		return;
	}
	auto Character = GetWorld()->SpawnActor<AAICharacter>(
		CharacterClass
		, GetComponentLocation()
		, GetComponentRotation()
		, CharacterSpawnParameters
	);
	if (Character == nullptr)
	{
		return;
	}

	Controller->Possess(Character);

	for (auto WeaponAndAmmo : WeaponAndAmmoArray)
	{
		if (!UMyUtilities::SpawnAndAddWeaponToCharacter(Character, WeaponAndAmmo.WeaponClass, WeaponAndAmmo.AmmoAmount))
		{
			UE_LOG(AISpawnAndOrderComponent, Display, TEXT("Failed to give the character weapon and ammo"));
		}
	}

	TArray<USceneComponent*> Children;
	GetChildrenComponents(/*bIncludeAllDescendants*/ false, Children);
	FMoveTargetSequenceTaskData MoveTargetSequenceTaskData;
	MoveTargetSequenceTaskData.OrderMoveTagetSequence.Reserve(Children.Num());
	for (auto& SceneComponent : Children)
	{
		if (auto MoveTargetComponent = Cast<UAIOrderMoveTargetComponent>(SceneComponent); MoveTargetComponent != nullptr)
		{
			MoveTargetSequenceTaskData.OrderMoveTagetSequence.Add(FMoveTargetData{
				MoveTargetComponent->GetComponentLocation()
				, MoveTargetComponent->GetComponentRotation()
				, MoveTargetComponent->TimeToStayInPosition
			});
		}
	}
	MoveTargetSequenceTaskData.FirstNonMandatoryOrderMoveTarget = FirstNonMandatoryOrderMoveTarget;
	MoveTargetSequenceTaskData.bLoopTargetSequence = LoopTargetSequence;

	Controller->SetOrderMoveTagetSequence(std::move(MoveTargetSequenceTaskData));
}

void UAISpawnAndOrderComponent::ScheduleSpawn()
{
	if (SpawnDelay < 0)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAISpawnAndOrderComponent::Spawn, SpawnDelay, false);
}

float UAISpawnAndOrderComponent::GetSpawnDelayRemaining() const
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		return -1.f;
	}
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle);
}
