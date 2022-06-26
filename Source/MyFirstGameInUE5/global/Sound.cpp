#include "Sound.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

static float s_musicVolume = 1; // volume in range [0, 1]
static UAudioComponent* s_musicComponent = nullptr;
static FTimerHandle s_musicTimerHandle = {};

void USound::SetMusicVolume(float volume)
{
	check(volume >= 0 && volume <= 1);
	s_musicVolume = volume;
	if (s_musicComponent != nullptr)
	{
		s_musicComponent->SetVolumeMultiplier(s_musicVolume);
	}
}

float USound::GetMusicVolume()
{
	return s_musicVolume;
}

void USound::PlayMusic(const UObject* WorldContextObject, USoundBase* Sound)
{
	StopMusic(WorldContextObject);
	if (auto soundWave = dynamic_cast<USoundWave*>(Sound); soundWave != nullptr)
	{
		soundWave->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
	}
	s_musicComponent = UGameplayStatics::SpawnSound2D(
		WorldContextObject
		, Sound
		, s_musicVolume
		, /*PitchMultiplier =*/ 1.f
		, /*StartTime =*/ 0.f
		, /*ConcurrencySettings* =*/ nullptr
		, /*bPersistAcrossLevelTransition =*/ true
		, /*bAutoDestroy =*/ false
	);
	UGameplayStatics::GetGameInstance(WorldContextObject)->RegisterReferencedObject(s_musicComponent);
}

void USound::PlayMusicInLoop(const UObject* WorldContextObject, USoundBase* Sound)
{
	PlayMusic(WorldContextObject, Sound);
	check(s_musicComponent != nullptr);
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetTimerManager().SetTimer(
		s_musicTimerHandle
		, []()
			{
				if (s_musicComponent != nullptr)
				{
					s_musicComponent->Play();
				}
			}
		, /*inRate =*/ s_musicComponent->GetSound()->GetDuration()
		, /*InbLoop =*/ true
	);
}

void USound::StopMusic(const UObject* WorldContextObject)
{
	if (s_musicComponent == nullptr)
	{
		return;
	}
	s_musicComponent->Stop();
	auto gameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	gameInstance->GetTimerManager().ClearTimer(s_musicTimerHandle);
	gameInstance->UnregisterReferencedObject(s_musicComponent);
	s_musicComponent->MarkPendingKill();
	s_musicComponent = nullptr;
}

void USound::PauseMusic(const UObject* WorldContextObject)
{
	if (s_musicComponent == nullptr)
	{
		return;
	}
	s_musicComponent->SetPaused(true);
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetTimerManager().PauseTimer(s_musicTimerHandle);
}

void USound::ResumeMusic(const UObject* WorldContextObject)
{
	if (s_musicComponent == nullptr)
	{
		return;
	}
	s_musicComponent->SetPaused(false);
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetTimerManager().UnPauseTimer(s_musicTimerHandle);
}