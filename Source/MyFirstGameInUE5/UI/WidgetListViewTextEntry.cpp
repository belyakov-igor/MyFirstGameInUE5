#include "UI/WidgetListViewTextEntry.h"

#include "Components/TextBlock.h"

void UWidgetListViewTextEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	auto WidgetListViewTextEntryData = Cast<UWidgetListViewTextEntryData>(ListItemObject);
	if (WidgetListViewTextEntryData == nullptr || txt_Text == nullptr)
	{
		return;
	}
	txt_Text->SetText(FText::AsCultureInvariant(WidgetListViewTextEntryData->String));
	txt_DateTime->SetText(FText::AsCultureInvariant("UTC " + WidgetListViewTextEntryData->DateTime.ToString()));
	WidgetListViewTextEntryData->EntryWidget = this;
}
