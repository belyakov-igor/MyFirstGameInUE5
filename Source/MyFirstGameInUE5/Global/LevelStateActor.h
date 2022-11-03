#pragma once

#include "Global/Utilities/MyUtilities.h"
#include "Global/MySaveGame.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LevelStateActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ALevelStateActor : public AActor, public ISavable
{
	GENERATED_BODY()

public:
	ALevelStateActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* MusicComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Music")
	bool bLoopMusic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TMap<FName, USoundBase*> MusicAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FName DefaultMusicAssetName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Category = "LevelState")
	int32 State = 0;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	bool IsMusicPlaying() { return bIsMusicPlaying; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	float GetMusicPlaybackTime() { return MusicPlaybackTime; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	FName GetCurrentMusicAssetName() { return CurrentMusicAssetName; }

	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetCurrentMusicAssetName(FName Name) { CurrentMusicAssetName = Name; }

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();


	UPROPERTY(BlueprintAssignable, Category = "Music")
	FSignalMulticastDynamicSignature MusicStartedPlaying;
	
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FSignalMulticastDynamicSignature MusicStoppedPlaying;

	UPROPERTY(BlueprintAssignable, Category = "LevelState")
	FSignalMulticastDynamicSignature AICharacterDied;

private:
	UPROPERTY(SaveGame)
	bool bIsMusicPlaying = false;
	
	UPROPERTY(SaveGame)
	float MusicPlaybackTime = 0.f;

	UPROPERTY(SaveGame)
	FName CurrentMusicAssetName = NAME_None;

	UFUNCTION()
	void OnStopPlayingMusic();

	UFUNCTION()
	void OnMusicPlaybackPercent(const USoundWave* PlayingSoundWave, const float PlaybackPercent);

	USoundBase* GetCurrentMusic() const;
};