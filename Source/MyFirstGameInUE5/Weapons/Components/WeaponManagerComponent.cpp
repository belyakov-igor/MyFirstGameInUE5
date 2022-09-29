#include "WeaponManagerComponent.h"

#include "Weapons/Actors/BaseWeapon.h"
#include "Weapons/Components/AmmoComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UWeaponManagerComponent::UWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Weapons.Init(nullptr, MaxWeaponSlot - MinWeaponSlot + 1);
}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponManagerComponent::AddWeapon(ABaseWeapon* Weapon)
{
	auto Character = Cast<ACharacter>(GetOwner());
	if (Weapon == nullptr || Character == nullptr)
	{
		return;
	}
	checkf(
		Weapon->Slot >= MinWeaponSlot && Weapon->Slot <= MaxWeaponSlot
		, TEXT("Weapon slot is out of range [%d, %d]"), MinWeaponSlot, MaxWeaponSlot
	);
	check (Weapons.Num() == MaxWeaponSlot - MinWeaponSlot + 1);
	uint32 SlotIndex = Weapon->Slot - MinWeaponSlot;
	if (Weapons[SlotIndex] != nullptr)
	{
		checkf(false, TEXT("Replacing weapon in a slot is not implemented"));
	}
	Weapons[SlotIndex] = Weapon;
	Weapon->AttachToOwner(Character);
	Weapon->OnAttackFinished.BindLambda([this]{ OnAttackFinished.Execute(); });
	if (Weapon->WeaponMesh != nullptr)
	{
		Weapon->WeaponMesh->SetVisibility(false);
	}
	if (auto AmmoComponent = Cast<UAmmoComponent>(Weapon->FindComponentByClass(UAmmoComponent::StaticClass())); AmmoComponent != nullptr)
	{
		auto Lambda = [this](int32, int32)
		{
			OnWeaponAndAmmoChanged.Execute();
		};
		AmmoComponent->ArsenalChanged.AddLambda(Lambda);
		AmmoComponent->ClipChanged.AddLambda(Lambda);
	}

	bool bOnWeaponAndAmmoChangedSignalNeeded = CurrentWeaponSlot == SlotIndex;
	if (GetCurrentWeapon() == nullptr)
	{
		SetCurrentWeaponSlot(SlotIndex);
	}
	if (bOnWeaponAndAmmoChangedSignalNeeded && OnWeaponAndAmmoChanged.IsBound())
	{
		OnWeaponAndAmmoChanged.Execute();
	}
}

void UWeaponManagerComponent::SetCurrentWeaponSlot(int32 Slot)
{
	checkf(
		Slot >= MinWeaponSlot && Slot <= MaxWeaponSlot
		, TEXT("Weapon slot is out of range [%d, %d]"), MinWeaponSlot, MaxWeaponSlot
	);
	if (Weapons[Slot] == nullptr)
	{
		return;
	}

	bool bSlotChanged = Weapons[CurrentWeaponSlot] == nullptr;
	if (
		CurrentWeaponSlot != Slot
		&& CurrentWeaponSlot >= MinWeaponSlot && CurrentWeaponSlot <= MaxWeaponSlot
		&& Weapons[CurrentWeaponSlot] != nullptr
		&& Weapons[CurrentWeaponSlot]->WeaponMesh != nullptr
	)
	{
		Weapons[CurrentWeaponSlot]->WeaponMesh->SetVisibility(false);
		bSlotChanged = true;
	}

	CurrentWeaponSlot = Slot;
	auto Weapon = Weapons[CurrentWeaponSlot];
	if (bWeaponIsVisible && Weapon->WeaponMesh != nullptr)
	{
		Weapon->WeaponMesh->SetVisibility(true);
	}

	if (bSlotChanged)
	{
		OnWeaponAndAmmoChanged.Execute();
		UGameplayStatics::SpawnSoundAttached(Weapon->EquipSound, Weapon->WeaponMesh, Weapon->HandGripSocketName);
	}

	return;
}

void UWeaponManagerComponent::SetNextWeapon()
{
	static constexpr auto CyclicIncr = [](int32& i){ return i = i == MaxWeaponSlot ? MinWeaponSlot : i + 1; };

	if (GetCurrentWeapon() == nullptr) // don't have any weapons
	{
		return;
	}

	auto Slot = CurrentWeaponSlot;
	while (Weapons[CyclicIncr(Slot)] == nullptr);
	SetCurrentWeaponSlot(Slot);
}

void UWeaponManagerComponent::SetPreviousWeapon()
{
	static constexpr auto CyclicDecr = [](int32& i){ return i = i == MinWeaponSlot ? MaxWeaponSlot : i - 1; };

	if (GetCurrentWeapon() == nullptr) // don't have any weapons
	{
		return;
	}

	auto Slot = CurrentWeaponSlot;
	while (Weapons[CyclicDecr(Slot)] == nullptr);
	SetCurrentWeaponSlot(Slot);
}

int32 UWeaponManagerComponent::GetCurrentWeaponIndex() const
{
	checkf(GetCurrentWeapon() != nullptr, TEXT("Add some weapons first"));
	return CurrentWeaponSlot;
}

ABaseWeapon* UWeaponManagerComponent::GetCurrentWeapon() const
{
	return Weapons[CurrentWeaponSlot];
}

void UWeaponManagerComponent::Reload()
{
	if (GetCurrentWeapon() == nullptr)
	{
		return;
	}
	auto Weapon = GetCurrentWeapon();
	auto AmmoComponent = Cast<UAmmoComponent>(Weapon->FindComponentByClass(UAmmoComponent::StaticClass()));
	if (AmmoComponent == nullptr)
	{
		return;
	}
	Weapon->EndAttack();
	AmmoComponent->Reload();
}

bool UWeaponManagerComponent::AttackIsBeingPerformed() const
{
	return GetCurrentWeapon() != nullptr && GetCurrentWeapon()->AttackIsBeingPerformed();
}

void UWeaponManagerComponent::SetIsCrouching(bool IsCrouching)
{
	for (auto Weapon : Weapons)
	{
		if (Weapon != nullptr)
		{
			Weapon->IsCrouching = IsCrouching;
		}
	}
}

void UWeaponManagerComponent::BeginAttack()
{
	auto Weapon = GetCurrentWeapon();
	if (Weapon != nullptr && !Weapon->AttackIsBeingPerformed())
	{
		Weapon->BeginAttack();
	}
}

void UWeaponManagerComponent::EndAttack()
{
	auto Weapon = GetCurrentWeapon();
	if (Weapon != nullptr && Weapon->AttackIsBeingPerformed())
	{
		Weapon->EndAttack();
	}
}

void UWeaponManagerComponent::SetWeaponVisibility(bool Visible)
{
	bWeaponIsVisible = Visible;
	if (auto Weapon = GetCurrentWeapon(); Weapon != nullptr && Weapon->WeaponMesh != nullptr)
	{
		Weapon->WeaponMesh->SetVisibility(Visible);
	}
}
