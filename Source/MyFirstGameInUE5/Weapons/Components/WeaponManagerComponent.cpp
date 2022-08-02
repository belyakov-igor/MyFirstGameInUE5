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
	auto Character = Cast<ACharacter>(GetOwner());
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

	return true;
}

int32 UWeaponManagerComponent::GetCurrentWeaponIndex() const
{
	checkf(!Weapons.IsEmpty(), TEXT("Add some weapons first"));
	return CurrentWeaponIndex;
}

void UWeaponManagerComponent::Reload()
{
	if (!HasValidWeapon())
	{
		return;
	}
	auto AmmoComponent = Cast<UAmmoComponent>(CurrentWeapon()->FindComponentByClass(UAmmoComponent::StaticClass()));
	if (AmmoComponent == nullptr)
	{
		return;
	}
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
