#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Settings.h"
#include "MyGameInstance.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	explicit UMyGameInstance() {}

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	UPROPERTY()
	USettings* Settings;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MyGameInstance", meta = (WorldContext = "WorldContextObject"))
	static UMyGameInstance* GetMyGameInstance(const UObject* WorldContextObject);

// Music { ////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	float GetMusicVolume();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayMusic(USoundBase* Sound);

	// Play track and repeat it until StopMusic is called
	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayMusicInLoop(USoundBase* Sound);

	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PauseMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void ResumeMusic();
// } Music ////////////////////////////////////////////////////////////////////////////////////////

// Game Sound { ///////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundClass* GameSoundSoundClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundClass* MusicSoundClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetGameSoundVolume(float Volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sound")
	float GetGameSoundVolume();
// } Game Sound ///////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName StartLevelName = NAME_None;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNewGame();

private:
	UPROPERTY()
	UAudioComponent* MusicComponent = nullptr;

	FTimerHandle MusicTimerHandle = {};
};
