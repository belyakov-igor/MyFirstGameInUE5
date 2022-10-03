#include "UI/LoadingOverlayWidget.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

void ULoadingOverlayWidget::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation);
	UGameplayStatics::PlaySound2D(GetWorld(), FadeInSound);
}

void ULoadingOverlayWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == FadeInAnimation)
	{
		FadeInAnimationFinished.Broadcast();
	}
}