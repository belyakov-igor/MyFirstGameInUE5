#include "UI/WidgetButtonWithText.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

bool UWidgetButtonWithText::Initialize()
{
	auto Ret = Super::Initialize();
	Init();
	return Ret;
}

void UWidgetButtonWithText::OnBtnPressed()
{
	OnButtonPressed.Broadcast();
}

void UWidgetButtonWithText::Init()
{
	if (Txt != nullptr)
	{
		Txt->SetText(Caption);
	}
	if (Btn != nullptr && !Btn->OnClicked.IsBound())
	{
		Btn->OnClicked.AddDynamic(this, &UWidgetButtonWithText::OnBtnPressed);
	}
}

void UWidgetButtonWithText::SetBackgroundColor(FLinearColor Color)
{
	if (Btn != nullptr)
	{
		Btn->SetBackgroundColor(Color);
	}
}
