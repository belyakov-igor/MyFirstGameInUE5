#include "UI/MenuLogic.h"

#include "UI/MenuWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

AMenuLogic::AMenuLogic()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMenuLogic::BeginPlay()
{
	Super::BeginPlay();
}

void AMenuLogic::Init(TMap<FName, TSubclassOf<UMenuWidget>> WidgetClasses, FName NewStartupWidgetName)
{
	checkf(!WidgetClasses.IsEmpty(), TEXT("WidgetClasses should not be empty"));
	for (auto [Name, WidgetClass] : WidgetClasses)
	{
		auto Widget = CreateWidget<UMenuWidget>(GetWorld(), WidgetClass, Name);
		check(Widget != nullptr);
		Widget->SetMenuLogic(this);
		Widgets.Add(Widget);
		if (Name == NewStartupWidgetName)
		{
			StartupWidgetName = NewStartupWidgetName;
		}
	}
}

void AMenuLogic::OpenUI()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
	ShowNextWidget(StartupWidgetName);
}

void AMenuLogic::CloseUI()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
	HideWidget();
	WidgetStack.Empty();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void AMenuLogic::ShowNextWidget(FName WidgetName)
{
	HideWidget();
	auto Index = Widgets.FindLastByPredicate([WidgetName](const UUserWidget* Widget){ return Widget->GetName() == WidgetName.ToString(); });
	checkf(Index != INDEX_NONE, TEXT("Widget with name \"%s\" is not found"), *WidgetName.ToString());
	auto Widget = Widgets[Index];
	WidgetStack.Add(Widget);
	ShowWidget(Index);
}

void AMenuLogic::ShowPreviousWidget()
{
	HideWidget();
	if (WidgetStack.IsEmpty())
	{
		return;
	}
	WidgetStack.RemoveAt(WidgetStack.Num() - 1);
	if (WidgetStack.IsEmpty())
	{
		return;
	}
	ShowWidget(WidgetStack.Num() - 1);
}

void AMenuLogic::ShowWidget(WidgetArray::SizeType Index)
{
	check(Index < Widgets.Num() && Index >= 0);
	auto Widget = Widgets[Index];
	Widget->AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0), Widget);
}

void AMenuLogic::HideWidget()
{
	if (WidgetStack.IsEmpty())
	{
		return;
	}
	WidgetStack.Top()->RemoveFromParent();
}
