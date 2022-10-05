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

	checkf(!WidgetClasses.IsEmpty(), TEXT("WidgetClasses should not be empty"));
	Widgets.Reserve(WidgetClasses.Num());
	WidgetStack.Reserve(WidgetClasses.Num());
	bool bStartupKeyFound = false;
	for (auto WidgetClass : WidgetClasses)
	{
		auto Widget = CreateWidget<UMenuWidget>(GetWorld(), WidgetClass);
		check(Widget != nullptr);
		checkf(
			FindWidget(Widget->Key) == INDEX_NONE
			, TEXT("There must be only one widget with key \"%s\" in WidgetClasses")
			, *Widget->Key.ToString()
		);

		Widget->SetMenuLogic(this);
		Widgets.Add(Widget);
		if (Widget->Key == StartupWidgetKey)
		{
			bStartupKeyFound = true;
		}
	}
	checkf(
		bStartupKeyFound
		, TEXT("WidgetClasses doesn't contain widget with specified StartupWidgetKey = \"%s\"")
		, *StartupWidgetKey.ToString()
	);
}

void AMenuLogic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CloseUI();
}

TArray<class UMenuWidget*>::SizeType AMenuLogic::FindWidget(FName Key)
{
	return Widgets.FindLastByPredicate([Key](const UMenuWidget* Widget) { return Widget->Key == Key; });
}

void AMenuLogic::OpenUI()
{
	if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); PlayerController != nullptr)
	{
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
	ShowNextWidget(StartupWidgetKey);
}

void AMenuLogic::CloseUI()
{
	HideWidget();
	WidgetStack.Empty();
	if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); PlayerController != nullptr)
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

void AMenuLogic::ShowNextWidget(FName WidgetKey)
{
	HideWidget();
	auto Index = FindWidget(WidgetKey);
	checkf(Index != INDEX_NONE, TEXT("Widget with key \"%s\" is not found"), *WidgetKey.ToString());
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

void AMenuLogic::ShowWidget(TArray<class UMenuWidget*>::SizeType Index)
{
	check(Index < Widgets.Num() && Index >= 0);
	auto Widget = Widgets[Index];
	Widget->AddToViewport();
}

void AMenuLogic::HideWidget()
{
	if (WidgetStack.IsEmpty())
	{
		return;
	}
	WidgetStack.Top()->RemoveFromParent();
}
