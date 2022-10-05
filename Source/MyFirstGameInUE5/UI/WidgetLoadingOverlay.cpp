#include "UI/WidgetLoadingOverlay.h"

#include "Global/MyGameInstance.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

void UWidgetLoadingOverlay::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation);
	if (auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)); GameInstance != nullptr)
	{
		GameInstance->StopMusic();
	}
	UGameplayStatics::PlaySound2D(GetWorld(), FadeInSound);
}

void UWidgetLoadingOverlay::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == FadeInAnimation)
	{
		FadeInAnimationFinished.Broadcast();
	}
}