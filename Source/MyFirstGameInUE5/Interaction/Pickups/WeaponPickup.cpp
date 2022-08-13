#include "Interaction/Pickups/WeaponPickup.h"

#include "Global/Utilities/MyUtilities.h"

#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "GameFramework/Pawn.h"

FText AWeaponPickup::FocusText()
{
	return FText::Format(FTextFormat(NSLOCTEXT("Interaction", "PickWeapon", "Take {0} (ammo: {1})")), WeaponName, AmmoAmount);
}

void AWeaponPickup::Interact(class APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		return;
	}
	auto WMComponent = Cast<UWeaponManagerComponent>(Pawn->GetDefaultSubobjectByName(RangedWeaponManagerComponentName));
	if (WMComponent == nullptr)
	{
		return;
	}
	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	auto Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
	if (Weapon == nullptr)
	{
		return;
	}
	auto AmmoComponent = Cast<UAmmoComponent>(Weapon->FindComponentByClass(UAmmoComponent::StaticClass()));
	if (AmmoComponent != nullptr)
	{
		AmmoComponent->MakeArsenalEmpty();
		AmmoComponent->MakeClipEmpty();
		AmmoComponent->IncreaseArsenal(AmmoAmount);
		AmmoComponent->ChangeClip();
	}
	WMComponent->AddWeapon(Weapon);
	Destroy();
	return;
}
