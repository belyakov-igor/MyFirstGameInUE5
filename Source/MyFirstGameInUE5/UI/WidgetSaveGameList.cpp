#include "UI/WidgetSaveGameList.h"

#include "Global/MyGameInstance.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

bool UWidgetSaveGameList::Initialize()
{
	auto Ret = Super::Initialize();

	Refresh();

	return Ret;
}

void UWidgetSaveGameList::Refresh()
{
	if (list_GameSaves == nullptr)
	{
		return;
	}

	list_GameSaves->ClearListItems();

	auto SaveGameSlots = UMyGameInstance::GetAllSaveGameSlots();

	txt_NoSavedGames->SetVisibility(SaveGameSlots.IsEmpty() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

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

void UWidgetSaveGameList::OnListViewSelectionChanged()
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

void UWidgetSaveGameList::OnListViewItemHovered(const UObject* Item, bool IsHovered)
{
	auto Data = Cast<UWidgetListViewTextEntryData>(Item);
	if (Data == nullptr)
	{
		return;
	}
	auto SlateVisibility = IsHovered ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	Data->EntryWidget->img_Hovered->SetVisibility(SlateVisibility);
}

bool UWidgetSaveGameList::ListIsEmpty() const
{
	if (list_GameSaves == nullptr)
	{
		return true;
	}
	return list_GameSaves->GetNumItems() == 0;
}

UWidgetListViewTextEntryData* UWidgetSaveGameList::GetSelectedEntryData() const
{
	if (list_GameSaves == nullptr)
	{
		return nullptr;
	}
	return Cast<UWidgetListViewTextEntryData>(list_GameSaves->GetSelectedItem());
}
