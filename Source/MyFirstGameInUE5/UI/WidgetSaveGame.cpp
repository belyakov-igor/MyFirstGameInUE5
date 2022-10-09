#include "UI/WidgetSaveGame.h"

#include "UI/WidgetButtonWithText.h"
#include "Global/MyGameInstance.h"
#include "UI/MenuLogic.h"
#include "UI/WidgetListViewTextEntry.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"

bool UWidgetSaveGame::Initialize()
{
	auto Ret = Super::Initialize();

	btn_Back->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::Back);
	btn_Save->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::SaveGameToSlotPickedInTheListView);
	btn_NewSave->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::ShowEditableText);
	btn_Delete->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::DeleteSaveForSlotPickedInTheListView);
	editTxt_EditableText->OnTextCommitted.AddDynamic(this, &UWidgetSaveGame::OnTextCommited);
	btn_EditableTextSave->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::SaveOnEditableTextCommited);
	btn_EditableTextCancel->OnButtonPressed.AddDynamic(this, &UWidgetSaveGame::HideEditableText);

	Refresh();

	return Ret;
}

void UWidgetSaveGame::Refresh()
{
	list_GameSaves->ClearListItems();

	auto SaveGameSlots = UMyGameInstance::GetAllSaveGameSlots();

	if (SaveGameSlots.IsEmpty())
	{
		btn_Save->SetIsEnabled(false);
		btn_Delete->SetIsEnabled(false);
		txt_NoSavedGames->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		btn_Save->SetIsEnabled(true);
		btn_Delete->SetIsEnabled(true);
		txt_NoSavedGames->SetVisibility(ESlateVisibility::Hidden);
	}

	for (const auto& SaveGameSlot : SaveGameSlots)
	{
		auto Data = NewObject<UWidgetListViewTextEntryData>(
			this
			, UWidgetListViewTextEntryData::StaticClass()
			, FName(SaveGameSlot.String)
		);
		Data->DateTime = SaveGameSlot.DateTime;
		Data->String = SaveGameSlot.String;
		list_GameSaves->AddItem(Data);
	}
	list_GameSaves->RegenerateAllEntries();
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
	if (list_GameSaves == nullptr)
	{
		return nullptr;
	}
	return Cast<UWidgetListViewTextEntryData>(list_GameSaves->GetSelectedItem());
}

void UWidgetSaveGame::OnTextCommited(const FText& Text, ETextCommit::Type CommitMethod)
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

void UWidgetSaveGame::OnListViewSelectionChanged()
{
	for (auto Item : list_GameSaves->GetListItems())
	{
		auto Data = Cast<UWidgetListViewTextEntryData>(Item);
		if (Data == nullptr)
		{
			continue;
		}
		auto SlateVisibility = list_GameSaves->IsItemSelected(Item) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		Data->EntryWidget->img_Selected->SetVisibility(SlateVisibility);
	}
}

void UWidgetSaveGame::OnListViewItemHovered(const UObject* Item, bool IsHovered)
{
	auto Data = Cast<UWidgetListViewTextEntryData>(Item);
	if (Data == nullptr)
	{
		return;
	}
	auto SlateVisibility = IsHovered ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	Data->EntryWidget->img_Hovered->SetVisibility(SlateVisibility);
}
