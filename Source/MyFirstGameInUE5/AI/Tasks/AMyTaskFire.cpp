#include "AI/Tasks/AMyTaskFire.h"

#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"
#include "Controllers/AIControllerBase.h"
#include "Global/Utilities/MyUtilities.h"

#include "BehaviorTree/BlackboardComponent.h"

UAMyTaskFire::UAMyTaskFire()
{
	bCreateNodeInstance = false;
	bNotifyTick = true;
	NodeName = "Fire";
}

EBTNodeResult::Type UAMyTaskFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	InitMemory(OwnerComp, NodeMemory);
	auto Result = Fire(OwnerComp, NodeMemory);
	if (Result != EBTNodeResult::InProgress)
	{
		Cleanup(OwnerComp);
	}
	return Result;
}

void UAMyTaskFire::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	auto Result = Fire(OwnerComp, NodeMemory);
	if (Result != EBTNodeResult::InProgress)
	{
		Cleanup(OwnerComp);
		FinishLatentTask(OwnerComp, Result);
	}
}

EBTNodeResult::Type UAMyTaskFire::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Cleanup(OwnerComp);
	return EBTNodeResult::Aborted;
}

uint16 UAMyTaskFire::GetInstanceMemorySize() const
{
	return sizeof(CharacterData);
}

void UAMyTaskFire::InitMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Data = GetCharacterData(OwnerComp);
	auto Memory = reinterpret_cast<TNodeMemory*>(NodeMemory);
	Memory->InitialTime = GetWorld()->GetTimeSeconds();
	Memory->InitialAmmo = Data.AmmoComponent->GetArsenalAmount() + Data.AmmoComponent->GetClipAmount();
}

UAMyTaskFire::CharacterData UAMyTaskFire::GetCharacterData(UBehaviorTreeComponent& OwnerComp)
{
	const auto Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	const auto Pawn = Controller == nullptr ? nullptr : Controller->GetPawn();
	auto WeaponManagerComponent =
		Pawn == nullptr
		? nullptr
		: Cast<UWeaponManagerComponent>(Pawn->GetDefaultSubobjectByName(RangedWeaponManagerComponentName))
	;
	auto CurrentWeapon =
		WeaponManagerComponent == nullptr
		? nullptr
		: WeaponManagerComponent->GetCurrentWeapon()
	;
	auto AmmoComponent =
		CurrentWeapon == nullptr
		? nullptr
		: Cast<UAmmoComponent>(CurrentWeapon->FindComponentByClass(UAmmoComponent::StaticClass()))
	;
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	auto Target =
		(Blackboard == nullptr || Controller == nullptr)
		? nullptr
		: Cast<AActor>(Blackboard->GetValueAsObject(Controller->EnemyActorKeyName))
	;

	return { Controller, Pawn, WeaponManagerComponent, AmmoComponent, Target };
}

EBTNodeResult::Type UAMyTaskFire::Fire(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MakeTraceIfNeeded(OwnerComp, NodeMemory);

	auto Data = GetCharacterData(OwnerComp);
	auto Memory = reinterpret_cast<TNodeMemory*>(NodeMemory);

	if (Data.Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Data.Controller->SetFocus(Data.Target);
	if (Memory->TraceSucceeded)
	{
		Data.WeaponManagerComponent->BeginAttack();
	}
	else
	{
		Data.WeaponManagerComponent->EndAttack();
	}
	return GetStatus(Data, Memory);
}

void UAMyTaskFire::MakeTraceIfNeeded(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Data = GetCharacterData(OwnerComp);
	auto Memory = reinterpret_cast<TNodeMemory*>(NodeMemory);

	if (
		static_cast<int32>(FMath::Floor((GetWorld()->GetTimeSeconds() - Memory->InitialTime) / TraceInterval + 1))
		<= Memory->TracesDone
	)
	{
		return;
	}

	auto Weapon = Data.WeaponManagerComponent->GetCurrentWeapon();
	if (!Weapon)
	{
		Memory->TraceSucceeded = false;
		return;
	}

	auto HitResult = Weapon->MakeTrace();
	Memory->TraceSucceeded = HitResult.GetActor() == Data.Target;
}

EBTNodeResult::Type UAMyTaskFire::GetStatus(const CharacterData& Data, TNodeMemory* Memory)
{
	if (
		Memory->InitialAmmo - Data.AmmoComponent->GetClipAmount() - Data.AmmoComponent->GetArsenalAmount() > MaxShots
		|| GetWorld()->GetTimeSeconds() - Memory->InitialTime > MaxTimeForThisTask
	)
	{
		return EBTNodeResult::Succeeded;
	}
	if (Data.AmmoComponent->GetClipAmount() == 0 && Data.AmmoComponent->GetArsenalAmount() == 0)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UAMyTaskFire::Cleanup(UBehaviorTreeComponent& OwnerComp)
{
	auto Data = GetCharacterData(OwnerComp);
	Data.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	Data.WeaponManagerComponent->EndAttack();
}
