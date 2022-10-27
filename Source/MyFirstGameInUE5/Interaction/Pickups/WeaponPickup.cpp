#include "Interaction/Pickups/WeaponPickup.h"

#include "Global/Utilities/MyUtilities.h"
#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"

#include "GameFramework/Pawn.h"

FText AWeaponPickup::FocusText_Implementation()
{
	return FText::Format(FTextFormat(NSLOCTEXT("Interaction", "PickWeapon", "Take {0} (ammo: {1})")), WeaponName, AmmoAmount);
}

void AWeaponPickup::Interact_Implementation(class APawn* Pawn)
{
	auto WMComponent = Cast<UWeaponManagerComponent>(Pawn->GetDefaultSubobjectByName(RangedWeaponManagerComponentName));
	if (WMComponent == nullptr)
	{
		return;
	}
	if (auto Weapon = WMComponent->GetWeaponOfClass(WeaponClass); Weapon != nullptr)
	{
		auto AmmoComponent = Cast<UAmmoComponent>(Weapon->FindComponentByClass(UAmmoComponent::StaticClass()));
		if (AmmoComponent != nullptr)
		{
			AmmoComponent->IncreaseArsenal(AmmoAmount);
		}
	}
	else if (!UMyUtilities::SpawnAndAddWeaponToCharacter(Pawn, WeaponClass, AmmoAmount))
	{
		return;
	}
	Destroy();
}
