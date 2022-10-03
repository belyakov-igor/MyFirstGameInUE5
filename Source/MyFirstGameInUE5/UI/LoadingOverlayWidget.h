#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LoadingOverlayWidget.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ULoadingOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayFadeInAnimation();

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FSignalMulticastDynamicSignature FadeInAnimationFinished;
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class USoundCue* FadeInSound = nullptr;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
};
