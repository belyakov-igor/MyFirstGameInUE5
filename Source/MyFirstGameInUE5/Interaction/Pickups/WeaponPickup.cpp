#include "Interaction/Pickups/WeaponPickup.h"

#include "Global/Utilities/MyUtilities.h"

#include "GameFramework/Pawn.h"

FText AWeaponPickup::FocusText()
{
	return FText::Format(FTextFormat(NSLOCTEXT("Interaction", "PickWeapon", "Take {0} (ammo: {1})")), WeaponName, AmmoAmount);
}

void AWeaponPickup::Interact(class APawn* Pawn)
{
	if (UMyUtilities::SpawnAndAddWeaponToCharacter(Pawn, WeaponClass, AmmoAmount))
	{
		Destroy();
	}
	return;
}
