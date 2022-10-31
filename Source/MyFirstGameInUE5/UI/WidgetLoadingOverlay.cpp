#include "UI/WidgetLoadingOverlay.h"

#include "Global/MyGameInstance.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

#include "Components/TextBlock.h"

void UWidgetLoadingOverlay::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation);
	UGameplayStatics::PlaySound2D(GetWorld(), FadeInSound);
}

void UWidgetLoadingOverlay::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == FadeInAnimation)
	{
		FadeInAnimationFinished.Broadcast();
	}
	else if (Animation == FadeOutAnimation)
	{
		FadeOutAnimationFinished.Broadcast();
	}
}

void UWidgetLoadingOverlay::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation);
}
