#pragma once

#include "Weapons/WeaponUtilities.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CharacterManHUDWidget.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UCharacterManHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateWeaponAndAmmo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateHealth();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateStamina();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void UpdateCrosshairVisibility(bool Visible);

protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterWeaponUIData(FWeaponUIData& CharacterFWeaponUIData) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterHealthUIData(int32& Health, int32& MaxHealth) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterStaminaUIData(int32& Stamina, int32& MaxStamina) const;

private:
	void UpdateWeaponAndAmmo_Implementation() {}
	void UpdateHealth_Implementation() {}
	void UpdateStamina_Implementation() {}
	void UpdateCrosshairVisibility_Implementation(bool) {}
};
