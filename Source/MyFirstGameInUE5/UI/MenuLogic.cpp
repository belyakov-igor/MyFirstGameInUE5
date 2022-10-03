#include "UI/MenuLogic.h"

#include "UI/MenuWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

AMenuLogic::AMenuLogic()
{
	PrimaryActorTick.bCanEverTick = false;
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
	if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); PlayerController != nullptr)
	{
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
	ShowNextWidget(StartupWidgetName);
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

void AMenuLogic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CloseUI();
}
