#include "Weapons/WeaponUtilities.h"

#include "Weapons/Components/WeaponManagerComponent.h"

void UWeaponUtilities::AddWeaponToCharacter(class ABaseWeapon* Weapon, class AActor* Character)
{
	check(Weapon != nullptr && Character != nullptr);
	UWeaponManagerComponent* WeaponManagerComponent =
		Cast<UWeaponManagerComponent>(Character->FindComponentByClass(UWeaponManagerComponent::StaticClass()))
	;
	if (WeaponManagerComponent == nullptr)
	{
		checkNoEntry();
		return;
	}
	WeaponManagerComponent->AddWeapon(Weapon);
}
