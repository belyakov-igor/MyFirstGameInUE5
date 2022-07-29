#include "WeaponManagerComponent.h"

#include "Weapons/Actors/BaseWeapon.h"

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
	Weapon->SetOwner(Character);
	Weapon->OnAttackFinished.BindLambda([this]{ OnAttackFinished.Execute(); });
}

bool UWeaponManagerComponent::AttackIsBeingPerformed() const
{
	return HasValidWeapon() && CurrentWeapon()->AttackIsBeingPerformed();
}

bool UWeaponManagerComponent::HasValidWeapon() const
{
	return !Weapons.IsEmpty() && CurrentWeapon() != nullptr;
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

ABaseWeapon* UWeaponManagerComponent::CurrentWeapon() const
{
	checkf(CurrentWeaponIndex >= 0 && CurrentWeaponIndex < Weapons.Num(), TEXT("Weapon index is out of bounds"));
	return Weapons[CurrentWeaponIndex];
}
