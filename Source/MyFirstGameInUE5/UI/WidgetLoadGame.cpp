#include "UI/WidgetLoadGame.h"

#include "UI/WidgetButtonWithText.h"
#include "Global/MyGameInstance.h"
#include "UI/MenuLogic.h"
#include "UI/WidgetSaveGameList.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"

bool UWidgetLoadGame::Initialize()
{
	auto Ret = Super::Initialize();

	btn_Back->OnButtonPressed.AddDynamic(this, &UWidgetLoadGame::Back);
	btn_Load->OnButtonPressed.AddDynamic(this, &UWidgetLoadGame::LoadGameFromSlotPickedInTheListView);
	btn_Delete->OnButtonPressed.AddDynamic(this, &UWidgetLoadGame::DeleteSaveForSlotPickedInTheListView);

	Refresh();

	return Ret;
}

void UWidgetLoadGame::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	Key = EMenuWidget_Load;

	wsglist_GameSaves->Refresh();
	btn_Load->SetIsEnabled(!wsglist_GameSaves->ListIsEmpty());
	btn_Delete->SetIsEnabled(!wsglist_GameSaves->ListIsEmpty());
}

void UWidgetLoadGame::Back()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowPreviousWidget();
}

void UWidgetLoadGame::LoadGameFromSlotPickedInTheListView()
{
	if (auto Data = GetPickedItemData(); Data != nullptr)
	{
		MenuLogic->CloseUI();
		UMyGameInstance::GetMyGameInstance(this)->LoadGame(Data->String);
	}
}

void UWidgetLoadGame::DeleteSaveForSlotPickedInTheListView()
{
	if (auto Data = GetPickedItemData(); Data != nullptr)
	{
		UMyGameInstance::GetMyGameInstance(this)->DeleteSave(Data->String);
	}
	Refresh();
}

UWidgetListViewTextEntryData* UWidgetLoadGame::GetPickedItemData() const
{
	if (wsglist_GameSaves == nullptr)
	{
		return nullptr;
	}
	return wsglist_GameSaves->GetSelectedEntryData();
}
