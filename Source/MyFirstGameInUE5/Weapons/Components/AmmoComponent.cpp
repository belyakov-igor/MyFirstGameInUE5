#include "Weapons/Components/AmmoComponent.h"

#include "Characters/PlayerCharacterBase.h"
#include "Weapons/Actors/BaseWeapon.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

	ClipAmmo = ClipCapacity;
	ArsenalAmmo = ArsenalCapacity;
}

int32 UAmmoComponent::Increase(
	int32 Amount
	, int32& Ammo
	, int32 Capacity
	, FSignalMulticastSignature& IsFull
	, FInt32ValueMulticastSignature& Changed
)
{
	checkf(Amount >= 0, TEXT("Amount must be non-negative"));
	int32 Ret = 0;
	if (Amount == 0)
	{
		return Ret;
	}
	if (Ammo == Capacity) // nothing changed, no broadcast
	{
		Ret = Amount;
		return Ret;
	}
	Ammo += Amount;
	if (Ammo >= Capacity)
	{
		Ret = Ammo - Capacity;
		Ammo = Capacity;
		IsFull.Broadcast();
	}
	Changed.Broadcast(Ammo);
	return Ret;
}

int32 UAmmoComponent::Decrease(
	int32 Amount
	, int32& Ammo
	, FSignalMulticastSignature& IsEmpty
	, FInt32ValueMulticastSignature& Changed
)
{
	checkf(Amount >= 0, TEXT("Amount must be non-negative"));
	int32 Ret = 0;
	if (Amount == 0)
	{
		return Ret;
	}
	if (Ammo == 0) // nothing changed, no broadcast
	{
		Ret = Amount;
		return Ret;
	}
	Ammo -= Amount;
	if (Ammo <= 0)
	{
		Ret = -Ammo;
		Ammo = 0;
		IsEmpty.Broadcast();
	}
	Changed.Broadcast(Ammo);
	return Ret;
}

int32 UAmmoComponent::IncreaseArsenal(int32 Amount)
{
	return Increase(Amount, ArsenalAmmo, ArsenalCapacity, ArsenalIsFull, ArsenalChanged);
}

int32 UAmmoComponent::DecreaseArsenal(int32 Amount)
{
	return Decrease(Amount, ArsenalAmmo, ArsenalIsEmpty, ArsenalChanged);
}


int32 UAmmoComponent::IncreaseClip(int32 Amount)
{
	return Increase(Amount, ClipAmmo, ClipCapacity, ClipIsFull, ClipChanged);
}

int32 UAmmoComponent::DecreaseClip(int32 Amount)
{
	return Decrease(Amount, ClipAmmo, ClipIsEmpty, ClipChanged);
}

void UAmmoComponent::Reload()
{
	if (ReloadAnimMontage != nullptr)
	{
		checkf(false, TEXT("Not implemented"));
		return;
	}

	// Do some poor substitute for reload animation
	auto Weapon = Cast<ABaseWeapon>(GetOwner());
	check(Weapon != nullptr);
	auto Character = Cast<APlayerCharacterBase>(Weapon->GetOwner());
	check(Character != nullptr);
	Character->AnimationSet = EPlayerCharacterBaseAnimationSet::Unarmed;

	GetWorld()->GetTimerManager().SetTimer(
		SomePoorSubstituteForReloadAnimationTimerHandle
		, [this]
			{
				auto Weapon = Cast<ABaseWeapon>(GetOwner());
				if (Weapon == nullptr)
				{
					return;
				}

				Weapon->SwitchCharacterToAnimationSet();
				GetWorld()->GetTimerManager().SetTimer(
					SomePoorSubstituteForReloadAnimationTimerHandle
					, this
					, &UAmmoComponent::ChangeClip
					, /*InRate*/ 0.25f
					, /*bInLoop*/ false
				);
			}
		, /*InRate*/ 0.25f
		, /*bInLoop*/ false
	);
}

void UAmmoComponent::ChangeClip()
{
	ClipAmmo = FMath::Min(ClipCapacity, ArsenalAmmo);
	ArsenalAmmo -= ClipAmmo;

	ArsenalChanged.Broadcast(ArsenalAmmo);
	ClipChanged.Broadcast(ClipAmmo);
	if (ArsenalAmmo == 0)
	{
		ArsenalIsEmpty.Broadcast();
	}
	if (ClipAmmo == ClipCapacity)
	{
		ClipIsFull.Broadcast();
	}
}
