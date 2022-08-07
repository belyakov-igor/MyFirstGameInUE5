#include "WeaponManagerComponent.h"

#include "Weapons/Actors/BaseWeapon.h"
#include "Characters/PlayerCharacterBase.h"
#include "Weapons/Components/AmmoComponent.h"

#include "GameFramework/Character.h"

UWeaponManagerComponent::UWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnAndAddDefaultWeapons();
}

void UWeaponManagerComponent::AddWeapon(ABaseWeapon* Weapon)
{
	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	if (Weapon == nullptr || Character == nullptr)
	{
		return;
	}
	Weapons.Add(Weapon);
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
			auto Character = Cast<APlayerCharacterBase>(GetOwner());
			if (Character != nullptr)
			{
				Character->OnWeaponAndAmmoChanged();
			}
		};
		AmmoComponent->ArsenalChanged.AddLambda(Lambda);
		AmmoComponent->ClipChanged.AddLambda(Lambda);
	}
}

bool UWeaponManagerComponent::SetCurrentWeapon(int32 index)
{
	checkf(index >= 0, TEXT("Index cannot be negative"));
	if (index >= Weapons.Num())
	{
		return false;
	}
	
	if (CurrentWeaponIndex != index && CurrentWeaponIndex >= 0 && CurrentWeaponIndex < Weapons.Num())
	{
		check(Weapons[CurrentWeaponIndex] != nullptr);
		Weapons[CurrentWeaponIndex]->WeaponMesh->SetVisibility(false);
	}

	CurrentWeaponIndex = index;
	check(Weapons[index] != nullptr);

	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	check(Character != nullptr);
	if (!Character->IsInNoAimingState() && Weapons[index]->WeaponMesh != nullptr)
	{
		Weapons[index]->SwitchCharacterToAnimationSet();
		Weapons[index]->WeaponMesh->SetVisibility(true);
	}

	Character->OnWeaponAndAmmoChanged();

	return true;
}

void UWeaponManagerComponent::SetNextWeapon()
{
	if (CurrentWeaponIndex == Weapons.Num() - 1)
	{
		SetCurrentWeapon(0);
		return;
	}
	SetCurrentWeapon(CurrentWeaponIndex + 1);
}

void UWeaponManagerComponent::SetPreviousWeapon()
{
	if (CurrentWeaponIndex == 0)
	{
		SetCurrentWeapon(Weapons.Num() - 1);
		return;
	}
	SetCurrentWeapon(CurrentWeaponIndex - 1);
}

int32 UWeaponManagerComponent::GetCurrentWeaponIndex() const
{
	checkf(!Weapons.IsEmpty(), TEXT("Add some weapons first"));
	return CurrentWeaponIndex;
}

ABaseWeapon* UWeaponManagerComponent::GetCurrentWeapon() const
{
	if (!HasValidWeapon())
	{
		return nullptr;
	}
	return CurrentWeapon();
}

void UWeaponManagerComponent::Reload()
{
	if (!HasValidWeapon())
	{
		return;
	}
	auto Weapon = CurrentWeapon();
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
	return !Weapons.IsEmpty() && CurrentWeapon()->AttackIsBeingPerformed();
}

bool UWeaponManagerComponent::HasValidWeapon() const
{
	return !Weapons.IsEmpty();
}

void UWeaponManagerComponent::SpawnAndAddDefaultWeapons()
{
	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	for (auto WeaponClass : DefaultWeapons)
	{
		auto Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
		if (Weapon == nullptr)
		{
			return;
		}
		AddWeapon(Weapon);
	}
}

void UWeaponManagerComponent::BeginAttack()
{
	if (Weapons.IsEmpty())
	{
		return;
	}
	auto Weapon = CurrentWeapon();
	if (!Weapon->AttackIsBeingPerformed())
	{
		Weapon->BeginAttack();
	}
}

void UWeaponManagerComponent::EndAttack()
{
	if (Weapons.IsEmpty())
	{
		return;
	}
	auto Weapon = CurrentWeapon();
	if (Weapon->AttackIsBeingPerformed())
	{
		Weapon->EndAttack();
	}
}

void UWeaponManagerComponent::BeginAim()
{
	if (Weapons.IsEmpty())
	{
		return;
	}
	auto Weapon = CurrentWeapon();
	Weapon->SwitchCharacterToAnimationSet();
	if (Weapon->WeaponMesh != nullptr)
	{
		Weapon->WeaponMesh->SetVisibility(true);
	}
}

void UWeaponManagerComponent::EndAim()
{
	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	check(Character != nullptr);
	Character->AnimationSet = EPlayerCharacterBaseAnimationSet::Unarmed;
	
	if (auto Weapon = CurrentWeapon(); Weapon->WeaponMesh != nullptr)
	{
		Weapon->WeaponMesh->SetVisibility(false);
	}
}

ABaseWeapon* UWeaponManagerComponent::CurrentWeapon() const
{
	checkf(CurrentWeaponIndex >= 0 && CurrentWeaponIndex < Weapons.Num(), TEXT("Weapon index is out of bounds"));
	check(Weapons[CurrentWeaponIndex] != nullptr);
	return Weapons[CurrentWeaponIndex];
}
