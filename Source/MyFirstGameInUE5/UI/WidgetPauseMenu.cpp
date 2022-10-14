#include "UI/WidgetPauseMenu.h"

#include "Global/MyGameInstance.h"

#include "UI/WidgetButtonWithText.h"
#include "UI/MenuLogic.h"

bool UWidgetPauseMenu::Initialize()
{
	auto Ret = Super::Initialize();

	Key = EMenuWidget_PauseMenu;

	btn_Continue->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnContinueClicked);
	btn_SaveGame->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnSaveClicked);
	btn_LoadGame->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnLoadClicked);
	btn_Options ->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnOptionsClicked);
	btn_MainMenu->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnMainMenuClicked);
	btn_QuitGame->OnButtonPressed.AddDynamic(this, &UWidgetPauseMenu::OnQuitGameClicked);

	return Ret;
}

void UWidgetPauseMenu::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	btn_LoadGame->SetIsEnabled(!UMyGameInstance::GetAllSaveGameSlots().IsEmpty());
}

void UWidgetPauseMenu::OnContinueClicked()
{
	check(MenuLogic != nullptr);
	MenuLogic->CloseUI();
}

void UWidgetPauseMenu::OnSaveClicked()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowNextWidget(EMenuWidget_Save);
}

void UWidgetPauseMenu::OnLoadClicked()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowNextWidget(EMenuWidget_Load);
}

void UWidgetPauseMenu::OnOptionsClicked()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowNextWidget(EMenuWidget_Options);
}

void UWidgetPauseMenu::OnMainMenuClicked()
{
	auto GameInstance = UMyGameInstance::GetMyGameInstance(this);
	check(GameInstance != nullptr);
	GameInstance->QuitToMainMenu();
}

void UWidgetPauseMenu::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
