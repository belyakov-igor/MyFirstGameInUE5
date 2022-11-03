#include "Global/LevelStateActor.h"

#include "Sound/SoundCue.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ALevelStateActor::ALevelStateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MusicComponent = CreateDefaultSubobject<UAudioComponent>("MusicComponent");
	MusicComponent->bIsUISound = true;
	MusicComponent->bAutoActivate = false;
	MusicComponent->bIsUISound = true;
}

void ALevelStateActor::BeginPlay()
{
	Super::BeginPlay();

	for (auto& [Name, Sound] : MusicAssets)
	{
		Sound->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
	}

	MusicComponent->OnAudioPlaybackPercent.AddDynamic(this, &ALevelStateActor::OnMusicPlaybackPercent);
	MusicComponent->SetSound(GetCurrentMusic());

	if (bIsMusicPlaying)
	{
		PlayMusic();
	}

	if (GetCurrentMusicAssetName() == NAME_None)
	{
		SetCurrentMusicAssetName(DefaultMusicAssetName);
	}

#if DO_CHECK
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), Actors);
		checkf(Actors.Num() == 1, TEXT("There should be only one LevelStateActor in a world"));
	}
#endif
}

void ALevelStateActor::PlayMusic()
{
	auto CurrentMusic = GetCurrentMusic();
	bool AssetChanged = MusicComponent->Sound != CurrentMusic;
	if (CurrentMusic == nullptr || MusicComponent->IsPlaying() && !AssetChanged)
	{
		return;
	}
	StopMusic();
	MusicComponent->SetSound(CurrentMusic);
	bIsMusicPlaying = true;
	MusicComponent->OnAudioFinished.AddDynamic(this, &ALevelStateActor::OnStopPlayingMusic);
	if (AssetChanged)
	{
		MusicPlaybackTime = 0.f;
	}
	MusicComponent->Play(MusicPlaybackTime);
	MusicStartedPlaying.Broadcast();
}

USoundBase* ALevelStateActor::GetCurrentMusic() const
{
	auto SoundPtr = MusicAssets.Find(CurrentMusicAssetName);
	return SoundPtr == nullptr ? nullptr : *SoundPtr;
}

void ALevelStateActor::StopMusic()
{
	if (!bIsMusicPlaying)
	{
		return;
	}
	MusicComponent->OnAudioFinished.RemoveDynamic(this, &ALevelStateActor::OnStopPlayingMusic);
	bIsMusicPlaying = false;
	MusicComponent->Stop();
	MusicStoppedPlaying.Broadcast();
}

void ALevelStateActor::OnStopPlayingMusic()
{
	if (bLoopMusic)
	{
		MusicComponent->Play(MusicPlaybackTime = 0.f);
	}
	else
	{
		MusicComponent->OnAudioFinished.RemoveDynamic(this, &ALevelStateActor::OnStopPlayingMusic);
		bIsMusicPlaying = false;
		MusicStoppedPlaying.Broadcast();
	}
}

void ALevelStateActor::OnMusicPlaybackPercent(const USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	MusicPlaybackTime = PlaybackPercent * PlayingSoundWave->Duration;
}
