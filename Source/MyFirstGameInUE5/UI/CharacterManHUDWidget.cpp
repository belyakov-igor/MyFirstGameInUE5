#include "UI/CharacterManHUDWidget.h"

#include "Global/Utilities/Components/ClampedIntegerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Actors/BaseWeapon.h"

#include "GameFramework/Pawn.h"

bool UCharacterManHUDWidget::GetCharacterWeaponUIData(FWeaponUIData& CharacterWeaponUIData) const
{
	auto Pawn = GetOwningPlayerPawn<APawn>();
	if (Pawn == nullptr)
	{
		return false;
	}
	auto WMComponent = Cast<UWeaponManagerComponent>(Pawn->GetDefaultSubobjectByName(RangedWeaponManagerComponentName));
	if (WMComponent == nullptr)
	{
		return false;
	}
	auto Weapon = WMComponent->GetCurrentWeapon();
	if (Weapon == nullptr)
	{
		return false;
	}
	CharacterWeaponUIData = Weapon->GetUIData();
	return true;
}

bool UCharacterManHUDWidget::GetCharacterHealthUIData(int32& Health, int32& MaxHealth) const
{
	auto Pawn = GetOwningPlayerPawn<APawn>();
	if (Pawn == nullptr)
	{
		return false;
	}
	auto HealthComponent = Cast<UClampedIntegerComponent>(Pawn->GetDefaultSubobjectByName(HealthComponentName));
	if (HealthComponent == nullptr)
	{
		return false;
	}
	Health = HealthComponent->GetValue();
	MaxHealth = HealthComponent->Max;
	return true;
}

bool UCharacterManHUDWidget::GetCharacterStaminaUIData(int32& Stamina, int32& MaxStamina) const
{
	auto Pawn = GetOwningPlayerPawn<APawn>();
	if (Pawn == nullptr)
	{
		return false;
	}
	auto StaminaComponent = Cast<UClampedIntegerComponent>(Pawn->GetDefaultSubobjectByName(StaminaComponentName));
	if (StaminaComponent == nullptr)
	{
		return false;
	}
	Stamina = StaminaComponent->GetValue();
	MaxStamina = StaminaComponent->Max;
	return true;
}
