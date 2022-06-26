// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API USound : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Music")
	static void SetMusicVolume(float volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	static float GetMusicVolume();

	UFUNCTION(BlueprintCallable, Category = "Music", meta = (WorldContext = "WorldContextObject"))
	static void PlayMusic(const UObject* WorldContextObject, USoundBase* Sound);

	// Play track and repeat it until StopMusic is called
	UFUNCTION(BlueprintCallable, Category = "Music", meta = (WorldContext = "WorldContextObject"))
	static void PlayMusicInLoop(const UObject* WorldContextObject, USoundBase* Sound);

	UFUNCTION(BlueprintCallable, Category = "Music", meta = (WorldContext = "WorldContextObject"))
	static void StopMusic(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Music", meta = (WorldContext = "WorldContextObject"))
	static void PauseMusic(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Music", meta = (WorldContext = "WorldContextObject"))
	static void ResumeMusic(const UObject* WorldContextObject);
	
};
