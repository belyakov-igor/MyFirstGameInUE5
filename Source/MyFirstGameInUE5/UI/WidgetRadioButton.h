#pragma once

#include "UI/WidgetButtonWithText.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetRadioButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonIndexMulticastDynamicSignature, int32, Index);

UCLASS()
class USignalWithIndex : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal")
	int32 Index;

	UPROPERTY(BlueprintAssignable, Category = "Signal")
	FButtonIndexMulticastDynamicSignature OnSignalRecieved;

	UFUNCTION(BlueprintCallable, Category = "Signal")
	void CallOnSignalReceived()
	{ OnSignalRecieved.Broadcast(Index); }
};

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetRadioButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintAssignable, Category = "UI")
	FButtonIndexMulticastDynamicSignature OnButtonPressed;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	int32 GetActiveButton() const { return ActiveIndex; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetActiveButton(int32 Index);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UHorizontalBox* HorizontalBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UWidgetButtonWithText> ButtonWithTextClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TArray<FText> Captions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 ActiveIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor ActiveColor = FLinearColor(0.8f, 0.8f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor InactiveColor = FLinearColor(0.1f, 0.1f, 0.1f);

private:
	UPROPERTY()
	TArray<UWidgetButtonWithText*> Buttons;

	UPROPERTY()
	TArray<USignalWithIndex*> IndexedSignals;
};
