#pragma once

#include "Global/Settings.h"
#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "MyGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FAsyncSaveGameToSlotMulticastDelegate, const FString& /*SlotName*/, const int32 /*UserIndex*/, bool /*succeeded*/);

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

// Game { /////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName StartLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName MainMenuLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FString QuickSaveSlotName;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void QuitToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game")
	static TArray<FDateTimeAndString> GetAllSaveGameSlots();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SaveGame(const FString& Slot);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void DeleteSave(const FString& Slot);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void MakeQuickSave();

	FSignalMulticastSignature OnSavingGameStarted;
	FAsyncSaveGameToSlotMulticastDelegate OnSavingGameFinished;
// } Game /////////////////////////////////////////////////////////////////////////////////////////

private:
	UPROPERTY()
	UAudioComponent* MusicComponent = nullptr;

	FTimerHandle MusicTimerHandle = {};

	FString DecoratedSaveSlotName(FString SaveSlotName);
	FString NonDecoratedSaveSlotName(FString SaveSlotName);
	inline static constexpr const char SaveSlotNameDecoration[] = "Save_";
	FAsyncSaveGameToSlotDelegate OnGameSavedNonMulticast;
	void OnGameSavedNonMulticastTriggered(const FString& SlotName, const int32 UserIndex, bool succeeded);
	inline static const FString SaveGameListSlot = "SaveGameListSlot";
};
