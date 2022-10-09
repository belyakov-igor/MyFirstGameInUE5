#include "UI/WidgetRadioButton.h"

#include "UI/WidgetButtonWithText.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

bool UWidgetRadioButton::Initialize()
{
	auto Ret = Super::Initialize();
	if (ButtonWithTextClass == nullptr)
	{
		return Ret;
	}

	HorizontalBox->ClearChildren();
	Buttons.Empty();
	IndexedSignals.Empty();

	for (const auto& Caption : Captions)
	{
		auto Button = CreateWidget<UWidgetButtonWithText>(this, ButtonWithTextClass, FName(Caption.ToString()));
		Button->Caption = Caption;
		Button->Init();
		auto HBSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(Button));
		HBSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Button->SetPadding(FMargin(10.f));
		Buttons.Add(Button);

		auto Signal = NewObject<UIndexedSignalObject>(this, UIndexedSignalObject::StaticClass());
		Signal->Index = IndexedSignals.Num();
		auto Index = IndexedSignals.Add(Signal);

		Button->OnButtonPressed.AddDynamic(IndexedSignals[Index], &UIndexedSignalObject::BroadcastIndexedSignal);
		IndexedSignals[Index]->IndexedSignal.AddDynamic(this, &UWidgetRadioButton::SetActiveButton);
	}

	if (!Buttons.IsEmpty())
	{
		SetActiveButton(ActiveIndex);
	}

	return Ret;
}

void UWidgetRadioButton::SetActiveButton(int32 Index)
{
	check(Index >= 0 && Index < Buttons.Num());
	ActiveIndex = Index;

	for (int32 i = 0; i != Buttons.Num(); ++i)
	{
		Buttons[i]->SetBackgroundColor(i == Index ? ActiveColor : InactiveColor);
	}

	OnButtonPressed.Broadcast(Index);
}
