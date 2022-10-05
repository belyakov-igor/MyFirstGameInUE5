#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetButtonWithText.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetButtonWithText : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintAssignable, Category = "UI")
	FSignalMulticastDynamicSignature OnButtonPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn))
	FText Caption;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void Init();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetBackgroundColor(FLinearColor Color);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Txt = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn = nullptr;

private:
	UFUNCTION()
	void OnBtnPressed();

};
