#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PlayerControllerBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class AMenuLogic> PauseMenuLogicClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class AMenuLogic* PauseMenuLogic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UWidgetProcessing> WidgetProcessingClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetProcessing* WidgetProcessingSave = nullptr;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void SwitchPause();
	void QuickSave();

private:
	void OnSavingGameStarted();
	void OnSavingGameFinished(const FString& /*SlotName*/, const int32 /*UserIndex*/, bool /*succeeded*/);
};
