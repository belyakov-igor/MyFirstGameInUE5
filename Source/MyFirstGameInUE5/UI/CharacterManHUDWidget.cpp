#include "UI/CharacterManHUDWidget.h"

#include "Characters/PlayerCharacterBase.h"

bool UCharacterManHUDWidget::GetCharacterWeaponUIData(FWeaponUIData& CharacterWeaponUIData) const
{
	auto Character = GetOwningPlayerPawn<APlayerCharacterBase>();
	if (Character == nullptr)
	{
		return false;
	}
	return Character->GetRangedWeaponUIData(CharacterWeaponUIData);
}

bool UCharacterManHUDWidget::GetCharacterHealthUIData(int32& Health, int32& MaxHealth) const
{
	auto Character = GetOwningPlayerPawn<APlayerCharacterBase>();
	if (Character == nullptr)
	{
		return false;
	}
	Character->GetHealthData(Health, MaxHealth);
	return true;
}

bool UCharacterManHUDWidget::GetCharacterStaminaUIData(int32& Stamina, int32& MaxStamina) const
{
	auto Character = GetOwningPlayerPawn<APlayerCharacterBase>();
	if (Character == nullptr)
	{
		return false;
	}
	Character->GetStaminaData(Stamina, MaxStamina);
	return true;
}
