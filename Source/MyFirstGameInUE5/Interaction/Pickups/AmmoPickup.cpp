#include "Interaction/Pickups/AmmoPickup.h"

#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "GameFramework/Pawn.h"

FText AAmmoPickup::FocusText_Implementation()
{
	return FText::Format(FTextFormat(NSLOCTEXT("Interaction", "PickAmmo", "Pick ammo for {0}: {1}")), WeaponName, Amount);
}

void AAmmoPickup::Interact_Implementation(class APawn* Pawn)
{
	TArray<UActorComponent*> WeaponManagerComponents;
	Pawn->GetComponents(UWeaponManagerComponent::StaticClass(), WeaponManagerComponents);
	for (auto Component : WeaponManagerComponents)
	{
		auto WMComponent = Cast<UWeaponManagerComponent>(Component);
		if (WMComponent == nullptr)
		{
			continue;
		}
		auto WeaponPtr =
			WMComponent->Weapons.FindByPredicate(
				[this](ABaseWeapon* Weapon){ return Weapon != nullptr && Weapon->IsA(WeaponClass); }
			)
		;
		if (WeaponPtr == nullptr)
		{
			continue;
		}
		check(*WeaponPtr != nullptr);
		auto AmmoComponent = Cast<UAmmoComponent>((*WeaponPtr)->FindComponentByClass(UAmmoComponent::StaticClass()));
		if (AmmoComponent == nullptr)
		{
			continue;
		}
		Amount = AmmoComponent->IncreaseArsenal(Amount);
		if (Amount <= 0)
		{
			Destroy();
		}
		return;
	}
}
