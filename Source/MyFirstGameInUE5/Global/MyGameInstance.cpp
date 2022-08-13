#include "MyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameUserSettings.h"

#include "Settings.h"

void UMyGameInstance::UMyGameInstance::Init()
{
	Settings = NewObject<USettings>();
	LoadSettings();

	auto GameUserSettings = UGameUserSettings::GetGameUserSettings();
	check(GameUserSettings != nullptr);
	GameUserSettings->ApplySettings(false);
}

void UMyGameInstance::SaveSettings()
{
	Settings->SaveConfig();
}

void UMyGameInstance::LoadSettings()
{
	Settings->LoadConfig();
}

UMyGameInstance* UMyGameInstance::GetMyGameInstance(const UObject* WorldContextObject)
{
	return static_cast<UMyGameInstance*>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UMyGameInstance::SetMusicVolume(float volume)
{
	check(volume >= 0 && volume <= 1);
	Settings->MusicVolume = volume;
	if (MusicComponent != nullptr)
	{
		MusicComponent->SetVolumeMultiplier(Settings->MusicVolume);
	}
}

float UMyGameInstance::GetMusicVolume()
{
	return Settings->MusicVolume;
}

void UMyGameInstance::PlayMusic(USoundBase* Sound)
{
	StopMusic();
	if (auto soundWave = dynamic_cast<USoundWave*>(Sound); soundWave != nullptr)
	{
		soundWave->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
	}
	MusicComponent = UGameplayStatics::SpawnSound2D(
		this
		, Sound
		, Settings->MusicVolume
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
	check(MusicComponent != nullptr);
	GetTimerManager().SetTimer(
		MusicTimerHandle
		, [this]()
		{
			if (MusicComponent != nullptr)
			{
				MusicComponent->Play();
			}
		}
		, /*inRate =*/ MusicComponent->GetSound()->GetDuration()
		, /*InbLoop =*/ true
	);
}

void UMyGameInstance::StopMusic()
{
	if (MusicComponent == nullptr)
	{
		return;
	}
	MusicComponent->Stop();
	GetTimerManager().ClearTimer(MusicTimerHandle);
	MusicComponent->MarkAsGarbage();
	MusicComponent = nullptr;
}

void UMyGameInstance::PauseMusic()
{
	if (MusicComponent == nullptr)
	{
		return;
	}
	MusicComponent->SetPaused(true);
	GetTimerManager().PauseTimer(MusicTimerHandle);
}

void UMyGameInstance::ResumeMusic()
{
	if (MusicComponent == nullptr)
	{
		return;
	}
	MusicComponent->SetPaused(false);
	GetTimerManager().UnPauseTimer(MusicTimerHandle);
}