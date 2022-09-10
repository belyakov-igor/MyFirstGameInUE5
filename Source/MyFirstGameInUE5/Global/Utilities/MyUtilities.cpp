#include "Global/Utilities/MyUtilities.h"

#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "Algo/Find.h"

void UMyUtilities::FindByValue(const TMap<FString, FString>& map, const FString& value, FString& ret, bool& succeeded)
{
	auto ptr = Algo::FindByPredicate(map, [&value](const auto& tuple){ return tuple.Value == value; });
	succeeded = ptr != nullptr;
	if (succeeded)
	{
		ret = ptr->Key;
	}
}

bool UMyUtilities::SpawnAndAddWeaponToCharacter(APawn* Character, TSubclassOf<class ABaseWeapon> WeaponClass, int32 AmmoAmount)
{
	if (Character == nullptr)
	{
		return false;
	}
	auto WMComponent = Cast<UWeaponManagerComponent>(Character->GetDefaultSubobjectByName(RangedWeaponManagerComponentName));
	if (WMComponent == nullptr)
	{
		return false;
	}
	auto World = Character->GetWorld();
	if (World == nullptr)
	{
		return false;
	}
	auto Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
	if (Weapon == nullptr)
	{
		return false;
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

	return true;
}
