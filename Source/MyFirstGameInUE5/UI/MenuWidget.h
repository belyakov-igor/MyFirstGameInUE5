#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MenuWidget.generated.h"

UENUM(BlueprintType)
enum EMenuWidget
{
	EMenuWidget_MainMenu          UMETA(DisplayName = "MainMenu"),
	EMenuWidget_PauseMenu         UMETA(DisplayName = "PauseMenu"),
	EMenuWidget_Save              UMETA(DisplayName = "Save"),
	EMenuWidget_Load              UMETA(DisplayName = "Load"),
	EMenuWidget_Options           UMETA(DisplayName = "Options"),
	EMenuWidget_OptionsGame       UMETA(DisplayName = "OptionsGame"),
	EMenuWidget_OptionsSound      UMETA(DisplayName = "OptionsSound"),
	EMenuWidget_OptionsGraphics   UMETA(DisplayName = "OptionsGraphics"),
	EMenuWidget_Invalid           UMETA(DisplayName = "Invalid")
};

UCLASS()
class MYFIRSTGAMEINUE5_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuLogic(class AMenuLogic* NewMenuLogic) { MenuLogic = NewMenuLogic; }

	virtual void Refresh() {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TEnumAsByte<EMenuWidget> Key = EMenuWidget_Invalid;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class AMenuLogic* MenuLogic = nullptr;
};
