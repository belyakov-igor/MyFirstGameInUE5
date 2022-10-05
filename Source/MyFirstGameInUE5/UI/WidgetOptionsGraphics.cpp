#include "UI/WidgetOptionsGraphics.h"

#include "UI/WidgetButtonWithText.h"
#include "UI/WidgetRadioButton.h"
#include "UI/MenuLogic.h"

#include "Components/ComboBoxString.h"
#include "Kismet/KismetStringLibrary.h"

bool UWidgetOptionsGraphics::Initialize()
{
	auto Ret = Super::Initialize();

	btn_Back   ->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::Back);
	btn_Optimal->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetOptimalSettings);

	SetSettingsBindings();
	SetUIFromSettings();

	return Ret;
}

void UWidgetOptionsGraphics::SetSettingsBindings()
{
	cbx_displayResolution->OnSelectionChanged.AddDynamic(this, &UWidgetOptionsGraphics::SetDisplayResolution);

	rbtn_WindowMode    ->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetWindowMode);
	rbtn_ViewDistance  ->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetViewDistance);
	rbtn_PostProcessing->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetPostProcessing);
	rbtn_AntiAliasing  ->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetAntiAliasing);
	rbtn_TextureQuality->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetTextureQuality);
	rbtn_ShadowQuality ->OnButtonPressed.AddDynamic(this, &UWidgetOptionsGraphics::SetShadowQuality);
}

void UWidgetOptionsGraphics::UnsetSettingsBindings()
{
	cbx_displayResolution->OnSelectionChanged.RemoveDynamic(this, &UWidgetOptionsGraphics::SetDisplayResolution);

	rbtn_WindowMode    ->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetWindowMode);
	rbtn_ViewDistance  ->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetViewDistance);
	rbtn_PostProcessing->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetPostProcessing);
	rbtn_AntiAliasing  ->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetAntiAliasing);
	rbtn_TextureQuality->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetTextureQuality);
	rbtn_ShadowQuality ->OnButtonPressed.RemoveDynamic(this, &UWidgetOptionsGraphics::SetShadowQuality);
}

void UWidgetOptionsGraphics::ApplyAndSaveSettings()
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->ApplySettings(/*bCheckForCommandLineOverrides*/false);
	Settings->SaveSettings();
}

void UWidgetOptionsGraphics::SetUIFromSettings()
{
	UnsetSettingsBindings();

	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);

	{
		auto Resolution = Settings->GetScreenResolution();
		auto String = FString::Printf(TEXT("%d%s%d"), Resolution.X, *ResolutionSplitSymbol, Resolution.Y);
		cbx_displayResolution->SetSelectedOption(String);
	}

	auto ToRadioButtonIndex = [](const auto& Map, auto Value, auto DefaultKey)
	{
		auto Key = Map.FindKey(Value);
		return Key != nullptr ? *Key : DefaultKey;
	};

	int32 WindowModeDefaultIndex = 0;
	{
		auto WindowModeDefaultIndex_ = RadioButtonIndexToWindowModeMap.FindKey(EWindowMode::Type::Windowed);
		if (WindowModeDefaultIndex_ != nullptr)
		{
			WindowModeDefaultIndex = *WindowModeDefaultIndex_;
		}
	}

	rbtn_WindowMode    ->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToWindowModeMap         , Settings->GetFullscreenMode()       , WindowModeDefaultIndex));
	rbtn_ViewDistance  ->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToViewDistanceQualityMap, Settings->GetViewDistanceQuality()  , 0                     ));
	rbtn_PostProcessing->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToQualityMap            , Settings->GetPostProcessingQuality(), 0                     ));
	rbtn_AntiAliasing  ->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToQualityMap            , Settings->GetAntiAliasingQuality()  , 0                     ));
	rbtn_TextureQuality->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToQualityMap            , Settings->GetTextureQuality()       , 0                     ));
	rbtn_ShadowQuality ->SetActiveButton(
		ToRadioButtonIndex(RadioButtonIndexToQualityMap            , Settings->GetShadowQuality()        , 0                     ));

	SetSettingsBindings();
}

void UWidgetOptionsGraphics::SetOptimalSettings()
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->RunHardwareBenchmark();
	Settings->ApplyHardwareBenchmarkResults();
	SetUIFromSettings();
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::Back()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowPreviousWidget();
}

void UWidgetOptionsGraphics::SetWindowMode(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetFullscreenMode(RadioButtonIndexToWindowModeMap[Index].GetValue());
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetDisplayResolution(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FString LeftS, RightS;
	if (!SelectedItem.Split(ResolutionSplitSymbol, &LeftS, &RightS))
	{
		return;
	}
	int32 ResolutionX = UKismetStringLibrary::Conv_StringToInt(LeftS);
	int32 ResolutionY = UKismetStringLibrary::Conv_StringToInt(RightS);
	if (ResolutionX == 0 || ResolutionY == 0)
	{
		return;
	}
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetScreenResolution(FIntPoint(ResolutionX, ResolutionY));
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetViewDistance(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetViewDistanceQuality(RadioButtonIndexToViewDistanceQualityMap[Index]);
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetPostProcessing(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetPostProcessingQuality(RadioButtonIndexToQualityMap[Index]);
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetAntiAliasing(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetAntiAliasingQuality(RadioButtonIndexToQualityMap[Index]);
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetTextureQuality(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetTextureQuality(RadioButtonIndexToQualityMap[Index]);
	ApplyAndSaveSettings();
}

void UWidgetOptionsGraphics::SetShadowQuality(int32 Index)
{
	auto Settings = UGameUserSettings::GetGameUserSettings();
	check(Settings != nullptr);
	Settings->SetShadowQuality(RadioButtonIndexToQualityMap[Index]);
	ApplyAndSaveSettings();
}
