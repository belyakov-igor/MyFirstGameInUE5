#include "MyGameInstance.h"

#include "Settings.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundClass.h"

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

void UMyGameInstance::SetMusicVolume(float Volume)
{
	checkf(Volume >= 0 && Volume <= 1, TEXT("Volume must be in range [0, 1]"));
	checkf(MusicSoundClass != nullptr, TEXT("MusicSoundClass should be specified"))
	Settings->MusicVolume = Volume;
	MusicSoundClass->Properties.Volume = Volume;UE_LOG(LogTemp, Warning, TEXT("SetMusicVolume"));
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

void UMyGameInstance::SetGameSoundVolume(float Volume)
{
	checkf(Volume >= 0 && Volume <= 1, TEXT("Volume must be in range [0, 1]"));
	checkf(GameSoundSoundClass != nullptr, TEXT("GameSoundSoundClass should be specified"))
	Settings->GameSoundVolume = Volume;
	GameSoundSoundClass->Properties.Volume = Volume; UE_LOG(LogTemp, Warning, TEXT("SetGameSoundVolume"));
}

float UMyGameInstance::GetGameSoundVolume()
{
	checkf(GameSoundSoundClass != nullptr, TEXT("GameSoundSoundClass should be specified"))
	return Settings->GameSoundVolume;
}

void UMyGameInstance::StartNewGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), StartLevelName);
}
