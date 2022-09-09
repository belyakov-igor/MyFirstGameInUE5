#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CharacterManHUDWidget.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UCharacterManHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateWeaponAndAmmo();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateHealth();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateStamina();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateCrosshairVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void ChangeInteractingHUDText(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void HealthDecreased();

protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterWeaponUIData(FWeaponUIData& CharacterFWeaponUIData) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterHealthUIData(int32& Health, int32& MaxHealth) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetCharacterStaminaUIData(int32& Stamina, int32& MaxStamina) const;
};
