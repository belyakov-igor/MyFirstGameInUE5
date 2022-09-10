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

void UAISpawnAndOrderComponent::ScheduleSpawn()
{
	auto Spawn = [this]()
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
		TArray<AAIControllerBase::MoveTarget> MoveTargets;
		MoveTargets.Reserve(Children.Num());
		for (auto& SceneComponent : Children)
		{
			if (auto MoveTargetComponent = Cast<UAIOrderMoveTargetComponent>(SceneComponent); MoveTargetComponent != nullptr)
			{
				MoveTargets.Add(AAIControllerBase::MoveTarget{
					MoveTargetComponent->GetComponentLocation()
					, MoveTargetComponent->GetComponentRotation()
					, MoveTargetComponent->TimeToStayInPosition
				});
			}
		}

		Controller->SetOrderMoveTagetSequence(MoveTargets, FirstNonMandatoryOrderMoveTarget, std::move(LoopTargetSequence));
	};
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Spawn, SpawnDelay, false);
}
