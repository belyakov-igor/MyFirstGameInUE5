#include "Controllers/PlayerControllerBase.h"

#include "UI/WidgetProcessing.h"

#include "UI/MenuLogic.h"
#include "Global/MyGameInstance.h"

void APlayerControllerBase::BeginPlay()
{
    Super::BeginPlay();

	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
    PauseMenuLogic = World->SpawnActor<AMenuLogic>(PauseMenuLogicClass);
    check(PauseMenuLogic != nullptr);

    if (WidgetProcessingClass != nullptr)
    {
        WidgetProcessingSave = CreateWidget<UWidgetProcessing>(GetWorld(), WidgetProcessingClass, "WidgetProcessingSave");
        WidgetProcessingSave->AddToViewport();

        auto GameInstance = UMyGameInstance::GetMyGameInstance(this);
        GameInstance->OnSavingGameStarted.AddUObject(this, &APlayerControllerBase::OnSavingGameStarted);
        GameInstance->OnSavingGameFinished.AddUObject(this, &APlayerControllerBase::OnSavingGameFinished);
    }
}

void APlayerControllerBase::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent == nullptr)
    {
        return;
    }

#if WITH_EDITOR
    InputComponent->BindAction("P_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::SwitchPause);
    InputComponent->BindAction("O_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::QuickSave);
    InputComponent->BindAction("L_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::QuickLoad);
#else
    InputComponent->BindAction("Esc_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::SwitchPause);
    InputComponent->BindAction("F5_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::QuickSave);
    InputComponent->BindAction("F9_Pressed", EInputEvent::IE_Pressed, this, &APlayerControllerBase::QuickLoad);
#endif
}

void APlayerControllerBase::SwitchPause()
{
    PauseMenuLogic->IsUIOpen() ? PauseMenuLogic->CloseUI() : PauseMenuLogic->OpenUI();
}

void APlayerControllerBase::QuickSave()
{
    UMyGameInstance::GetMyGameInstance(this)->MakeQuickSave();
}

void APlayerControllerBase::QuickLoad()
{
    UMyGameInstance::GetMyGameInstance(this)->MakeQuickLoad();
}

void APlayerControllerBase::OnSavingGameStarted()
{
    check(WidgetProcessingSave != nullptr);
    WidgetProcessingSave->StopAllAnimations();
    WidgetProcessingSave->PlayAnimation(WidgetProcessingSave->FadeInAnimation);
}

void APlayerControllerBase::OnSavingGameFinished(const FString& /*SlotName*/, const int32 /*UserIndex*/, bool /*succeeded*/)
{
    check(WidgetProcessingSave != nullptr);
    WidgetProcessingSave->StopAllAnimations();
    WidgetProcessingSave->PlayAnimation(WidgetProcessingSave->FadeOutAnimation);
}
