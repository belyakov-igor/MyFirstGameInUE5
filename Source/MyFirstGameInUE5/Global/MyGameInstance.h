#pragma once

#include "Global/Settings.h"
#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "MyGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FAsyncSaveGameToSlotMulticastDelegate, const FString& /*SlotName*/, const int32 /*UserIndex*/, bool /*succeeded*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAsyncLoadGameFromSlotMulticastDelegate, const FString& /*SlotName*/, const int32 /*UserIndex*/, USaveGame*);

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
	USettings* Settings = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MyGameInstance", meta = (WorldContext = "WorldContextObject"))
	static UMyGameInstance* GetMyGameInstance(const UObject* WorldContextObject);

// Music { ////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundClass* MusicSoundClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	float GetMusicVolume();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();
// } Music ////////////////////////////////////////////////////////////////////////////////////////

// Game Sound { ///////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundClass* GameSoundSoundClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetGameSoundVolume(float Volume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sound")
	float GetGameSoundVolume();
// } Game Sound ///////////////////////////////////////////////////////////////////////////////////

// Game { /////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName StartLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName StartLevelPlayerStartName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FName MainMenuLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FString QuickSaveSlotName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<class UWidgetLoadingOverlay> LoadingOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<class UWidgetLoadingOverlay> GameOverOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<class UWidgetLoadingOverlay> BlackScreenOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game")
	class ALevelStateActor* GetLevelStateActor();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void PauseGame(bool Pause);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void QuitToMainMenu();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game")
	static TArray<FDateTimeAndString> GetAllSaveGameSlots();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SaveGame(const FString& Slot);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void DeleteSave(const FString& Slot);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void MakeQuickSave();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LoadGame(const FString& Slot);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void MakeQuickLoad();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LoadLastSave();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RemoveTransformFromSaveGameForGlobalActor(FName GlobalActorName, FName LevelName);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SendPlayerToPlayerStart(FName LevelName, FName PlayerStartName);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void GameOver();

	// called from AMyGameModeBase::InitGame
	void OnLevelLoaded();
	void OnMainMenuLoaded();

	FSignalMulticastSignature OnSavingGameStarted;
	FAsyncSaveGameToSlotMulticastDelegate OnSavingGameFinished;
	FSignalMulticastSignature OnLoadingGameStarted;
	FSignalMulticastSignature OnLoadingGameFinished;
// } Game /////////////////////////////////////////////////////////////////////////////////////////

private:
	FString DecoratedSaveSlotName(FString SaveSlotName);
	FString NonDecoratedSaveSlotName(FString SaveSlotName);
	inline static constexpr const char SaveSlotNameDecoration[] = "Save_";
	void OnGameSavedNonMulticastTriggered(const FString& SlotName, const int32 UserIndex, bool succeeded);
	void OnGameLoadedNonMulticastTriggered(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);
	inline static const FString SaveGameListSlot = "SaveGameListSlot";

	UPROPERTY()
	class UMySaveGame* CurrentSave = nullptr;

	enum class ELoadingOverlayCallbackType { NewGame, LoadGame, SendPlayerToAnotherLevel, Invalid };
	ELoadingOverlayCallbackType LoadingOverlayCallbackType = ELoadingOverlayCallbackType::Invalid;

	FString LoadGameImplSlot;

	enum class EMainMenuTransitionOverlayType { Regular, GameOver, Invalid };
	EMainMenuTransitionOverlayType MainMenuTransitionOverlayType = EMainMenuTransitionOverlayType::Invalid;

	UFUNCTION() void OnLoadingOverlayFadeInAnimationFinished();
	UFUNCTION() void OnLoadingOverlayFadeOutAnimationFinished();
	UFUNCTION() void OnGameOverOverlayFadeInAnimationFinished();
	UFUNCTION() void OnGameOverOverlayFadeOutAnimationFinished();
	UFUNCTION() void OnBlackScreenOverlayFadeInAnimationFinished();
	UFUNCTION() void OnBlackScreenOverlayFadeOutAnimationFinished();

	void QuitToMainMenuImpl(bool IsGameOver);

	UPROPERTY()
	UWidgetLoadingOverlay* LoadingOverlayWidget = nullptr;

	UPROPERTY()
	UWidgetLoadingOverlay* GameOverOverlayWidget = nullptr;

	UPROPERTY()
	UWidgetLoadingOverlay* BlackScreenOverlayWidget = nullptr;

	class UMySaveGame* InitNewGameSaveObject();
};
