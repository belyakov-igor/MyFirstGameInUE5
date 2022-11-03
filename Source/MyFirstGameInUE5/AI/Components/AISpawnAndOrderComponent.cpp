#include "AI/Components/AISpawnAndOrderComponent.h"

#include "AI/Actors/AISpawnAndOrderActor.h"

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

	Controller->GroupId = GroupId;

	Controller->Possess(Character);

	for (auto WeaponAndAmmo : WeaponAndAmmoArray)
	{
		if (!UMyUtilities::SpawnAndAddWeaponToCharacter(Character, WeaponAndAmmo.WeaponClass, WeaponAndAmmo.AmmoAmount))
		{
			UE_LOG(AISpawnAndOrderComponent, Display, TEXT("Failed to give the character weapon and ammo"));
		}
	}

	const auto Parent = Cast<AAISpawnAndOrderActor>(GetOwner());
	checkf(Parent != nullptr, TEXT("Place this component only into AISpawnAndOrderActor."));
	FMoveTargetSequenceTaskData MoveTargetSequenceTaskData;
	MoveTargetSequenceTaskData.OrderMoveTagetSequence.Reserve(OrderMoveTargetNames.Num());
	const auto& OrderMoveTargetsAvailable = Parent->GetOrderMoveTargets();
	for (const auto Name : OrderMoveTargetNames)
	{
		auto OrderMoveTargetPtr = OrderMoveTargetsAvailable.Find(Name);
		checkf(OrderMoveTargetPtr != nullptr, TEXT("There is no OrderMoveTarget with this name: %s"), *Name.ToString());
		auto OrderMoveTarget = *OrderMoveTargetPtr;
		check(OrderMoveTarget != nullptr);
		MoveTargetSequenceTaskData.OrderMoveTagetSequence.Add(
			FMoveTargetData{
				OrderMoveTarget->GetComponentLocation()
				, OrderMoveTarget->GetComponentRotation()
				, OrderMoveTarget->TimeToStayInPosition
			}
		);
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
	if (SpawnDelay == 0.f)
	{
		Spawn();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAISpawnAndOrderComponent::Spawn, SpawnDelay, false);
	}
}

float UAISpawnAndOrderComponent::GetSpawnDelayRemaining() const
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		return -1.f;
	}
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle);
}
