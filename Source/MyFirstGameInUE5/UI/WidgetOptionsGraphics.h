#pragma once

#include "UI/MenuWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/SlateEnums.h"

#include "WidgetOptionsGraphics.generated.h"

namespace EWindowMode
{
	enum Type;
}

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetOptionsGraphics : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Back = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Optimal = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UComboBoxString* cbx_displayResolution = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_WindowMode = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_ViewDistance = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_PostProcessing = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_AntiAliasing = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_TextureQuality = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetRadioButton* rbtn_ShadowQuality = nullptr;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TMap<int32, TEnumAsByte<EWindowMode::Type>> RadioButtonIndexToWindowModeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TMap<int32, int32> RadioButtonIndexToViewDistanceQualityMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TMap<int32, int32> RadioButtonIndexToQualityMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString ResolutionSplitSymbol;

private:
	UFUNCTION()
	void Back();

	UFUNCTION()
	void SetOptimalSettings();

	UFUNCTION()
	void SetWindowMode(int32 Index);

	UFUNCTION()
	void SetDisplayResolution(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void SetViewDistance(int32 Index);

	UFUNCTION()
	void SetPostProcessing(int32 Index);

	UFUNCTION()
	void SetAntiAliasing(int32 Index);

	UFUNCTION()
	void SetTextureQuality(int32 Index);

	UFUNCTION()
	void SetShadowQuality(int32 Index);

	void SetUIFromSettings();

	void SetSettingsBindings();
	void UnsetSettingsBindings();

	void ApplyAndSaveSettings();
};
