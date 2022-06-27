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
	explicit UMyGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	UPROPERTY()
	USettings* settings;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MyGameInstance", meta = (WorldContext = "WorldContextObject"))
	static UMyGameInstance* GetMyGameInstance(const UObject* WorldContextObject);

// Sound { ////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetMusicVolume(float volume);

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
// } Sound ////////////////////////////////////////////////////////////////////////////////////////

private:
	UPROPERTY()
	UAudioComponent* musicComponent = nullptr;

	FTimerHandle musicTimerHandle = {};
};
