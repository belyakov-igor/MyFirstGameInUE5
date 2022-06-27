#include "MyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "Settings.h"

UMyGameInstance::UMyGameInstance()
{
	settings = NewObject<USettings>();
	LoadSettings();
}

void UMyGameInstance::SaveSettings()
{
	settings->SaveConfig();
}

void UMyGameInstance::LoadSettings()
{
	settings->LoadConfig();
}

UMyGameInstance* UMyGameInstance::GetMyGameInstance(const UObject* WorldContextObject)
{
	return static_cast<UMyGameInstance*>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UMyGameInstance::SetMusicVolume(float volume)
{
	check(volume >= 0 && volume <= 1);
	settings->musicVolume = volume;
	if (musicComponent != nullptr)
	{
		musicComponent->SetVolumeMultiplier(settings->musicVolume);
	}
}

float UMyGameInstance::GetMusicVolume()
{
	return settings->musicVolume;
}

void UMyGameInstance::PlayMusic(USoundBase* Sound)
{
	StopMusic();
	if (auto soundWave = dynamic_cast<USoundWave*>(Sound); soundWave != nullptr)
	{
		soundWave->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
	}
	musicComponent = UGameplayStatics::SpawnSound2D(
		this
		, Sound
		, settings->musicVolume
		, /*PitchMultiplier =*/ 1.f
		, /*StartTime =*/ 0.f
		, /*ConcurrencySettings* =*/ nullptr
		, /*bPersistAcrossLevelTransition =*/ true
		, /*bAutoDestroy =*/ false
	);
}

void UMyGameInstance::PlayMusicInLoop(USoundBase* Sound)
{
	PlayMusic(Sound);
	check(musicComponent != nullptr);
	GetTimerManager().SetTimer(
		musicTimerHandle
		, [this]()
		{
			if (musicComponent != nullptr)
			{
				musicComponent->Play();
			}
		}
		, /*inRate =*/ musicComponent->GetSound()->GetDuration()
		, /*InbLoop =*/ true
	);
}

void UMyGameInstance::StopMusic()
{
	if (musicComponent == nullptr)
	{
		return;
	}
	musicComponent->Stop();
	GetTimerManager().ClearTimer(musicTimerHandle);
	musicComponent->MarkPendingKill();
	musicComponent = nullptr;
}

void UMyGameInstance::PauseMusic()
{
	if (musicComponent == nullptr)
	{
		return;
	}
	musicComponent->SetPaused(true);
	GetTimerManager().PauseTimer(musicTimerHandle);
}

void UMyGameInstance::ResumeMusic()
{
	if (musicComponent == nullptr)
	{
		return;
	}
	musicComponent->SetPaused(false);
	GetTimerManager().UnPauseTimer(musicTimerHandle);
}