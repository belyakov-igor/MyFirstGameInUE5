#pragma once

#include "UI/MenuWidget.h"

#include "CoreMinimal.h"

#include "WidgetPauseMenu.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetPauseMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_Continue = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_SaveGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_LoadGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_Options = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_MainMenu = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UWidgetButtonWithText* btn_QuitGame = nullptr;

	virtual void Refresh_Implementation() override;

private:
	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnSaveClicked();

	UFUNCTION()
	void OnLoadClicked();

	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitGameClicked();
};
