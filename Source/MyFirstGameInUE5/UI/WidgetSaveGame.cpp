#include "UI/WidgetSaveGame.h"

#include "UI/WidgetButtonWithText.h"
#include "Global/MyGameInstance.h"
#include "UI/MenuLogic.h"
#include "UI/WidgetSaveGameList.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"

bool UWidgetSaveGame::Initialize()
{
	auto Ret = Super::Initialize();

	Key = EMenuWidget_Save;

	btn_Back->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::Back);
	btn_Save->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::SaveGameToSlotPickedInTheListView);
	btn_NewSave->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::ShowEditableText);
	btn_Delete->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::DeleteSaveForSlotPickedInTheListView);
	editTxt_EditableText->OnTextCommitted.AddDynamic(this, &UWidgetSaveGame::OnTextCommitted);
	btn_EditableTextSave->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::SaveOnEditableTextCommited);
	btn_EditableTextCancel->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::HideEditableText);

	Refresh();

	return Ret;
}

void UWidgetSaveGame::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	if (wsglist_GameSaves == nullptr)
	{
		return;
	}
	wsglist_GameSaves->Refresh();
	btn_Save->SetIsEnabled(!wsglist_GameSaves->ListIsEmpty());
	btn_Delete->SetIsEnabled(!wsglist_GameSaves->ListIsEmpty());
}

void UWidgetSaveGame::Back()
{
	check(MenuLogic != nullptr);
	MenuLogic->ShowPreviousWidget();
}

void UWidgetSaveGame::SaveGameToSlot(FString SlotName)
{
	MenuLogic->CloseUI();
	UMyGameInstance::GetMyGameInstance(this)->SaveGame(SlotName);
}

void UWidgetSaveGame::SaveGameToSlotPickedInTheListView()
{
	if (auto Data = GetPickedItemData(); Data != nullptr)
	{
		SaveGameToSlot(Data->String);
	}
}

void UWidgetSaveGame::DeleteSaveForSlotPickedInTheListView()
{
	if (auto Data = GetPickedItemData(); Data != nullptr)
	{
		UMyGameInstance::GetMyGameInstance(this)->DeleteSave(Data->String);
	}
	Refresh();
}

UWidgetListViewTextEntryData* UWidgetSaveGame::GetPickedItemData() const
{
	if (wsglist_GameSaves == nullptr)
	{
		return nullptr;
	}
	return wsglist_GameSaves->GetSelectedEntryData();
}

void UWidgetSaveGame::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter && !Text.IsEmpty())
	{
		HideEditableText();
		SaveGameToSlot(Text.ToString());
	}
}

void UWidgetSaveGame::SaveOnEditableTextCommited()
{
	auto SaveSlot = editTxt_EditableText->GetText().ToString();
	if (!SaveSlot.IsEmpty())
	{
		HideEditableText();
		SaveGameToSlot(SaveSlot);
	}
}
