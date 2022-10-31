#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetLoadingOverlay.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetLoadingOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayFadeInAnimation();

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FSignalMulticastDynamicSignature FadeInAnimationFinished;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayFadeOutAnimation();

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FSignalMulticastDynamicSignature FadeOutAnimationFinished;
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class USoundCue* FadeInSound = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnimation = nullptr;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
};
