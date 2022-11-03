#include "MyGameInstance.h"

#include "Settings.h"
#include "Global/MySaveGame.h"
#include "Global/MyGameModeBase.h"
#include "UI/WidgetLoadingOverlay.h"
#include "Characters/PlayerCharacter.h"
#include "Controllers/PlayerControllerBase.h"
#include "Global/LevelStateActor.h"

#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundClass.h"
#include "GameFramework/PlayerStart.h"

DEFINE_LOG_CATEGORY_STATIC(LogMyGameInstance, All, All);

void UMyGameInstance::UMyGameInstance::Init()
{
	Settings = NewObject<USettings>();
	LoadSettings();

	auto GameUserSettings = UGameUserSettings::GetGameUserSettings();
	check(GameUserSettings != nullptr);
	GameUserSettings->ApplySettings(false);

	if (Cast<UMySaveGameList>(UGameplayStatics::LoadGameFromSlot(SaveGameListSlot, 0)) == nullptr)
	{
		UMySaveGameList* SaveGameList = Cast<UMySaveGameList>(UGameplayStatics::CreateSaveGameObject(UMySaveGameList::StaticClass()));
		if (SaveGameList == nullptr || !UGameplayStatics::SaveGameToSlot(SaveGameList, SaveGameListSlot, 0))
		{
			UE_LOG(LogMyGameInstance, Error, TEXT("Failed to initialize SaveGameList."));
		}
	}

	if (LoadingOverlayWidgetClass != nullptr)
	{
		LoadingOverlayWidget = CreateWidget<UWidgetLoadingOverlay>(this, LoadingOverlayWidgetClass, "LoadingOverlay");
		LoadingOverlayWidget->FadeInAnimationFinished.AddDynamic(this, &UMyGameInstance::OnLoadingOverlayFadeInAnimationFinished);
		LoadingOverlayWidget->FadeOutAnimationFinished.AddDynamic(this, &UMyGameInstance::OnLoadingOverlayFadeOutAnimationFinished);
	}
	if (GameOverOverlayWidgetClass != nullptr)
	{
		GameOverOverlayWidget = CreateWidget<UWidgetLoadingOverlay>(this, GameOverOverlayWidgetClass, "GameOverOverlay");
		GameOverOverlayWidget->FadeInAnimationFinished.AddDynamic(this, &UMyGameInstance::OnGameOverOverlayFadeInAnimationFinished);
		GameOverOverlayWidget->FadeOutAnimationFinished.AddDynamic(this, &UMyGameInstance::OnGameOverOverlayFadeOutAnimationFinished);
	}
	if (BlackScreenOverlayWidgetClass != nullptr)
	{
		BlackScreenOverlayWidget = CreateWidget<UWidgetLoadingOverlay>(this, BlackScreenOverlayWidgetClass, "BlackScreenOverlay");
		BlackScreenOverlayWidget->FadeInAnimationFinished.AddDynamic(this, &UMyGameInstance::OnBlackScreenOverlayFadeInAnimationFinished);
		BlackScreenOverlayWidget->FadeOutAnimationFinished.AddDynamic(this, &UMyGameInstance::OnBlackScreenOverlayFadeOutAnimationFinished);
	}
}

void UMyGameInstance::SaveSettings()
{
	check(Settings != nullptr);

	Settings->MusicVolume = GetMusicVolume();
	Settings->GameSoundVolume = GetGameSoundVolume();

	Settings->SaveConfig();
}

void UMyGameInstance::LoadSettings()
{
	check(Settings != nullptr);

	Settings->LoadConfig();

	SetMusicVolume(Settings->MusicVolume);
	SetGameSoundVolume(Settings->GameSoundVolume);
}

UMyGameInstance* UMyGameInstance::GetMyGameInstance(const UObject* WorldContextObject)
{
	auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(GameInstance != nullptr);
	return GameInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UMyGameInstance::SetMusicVolume(float Volume)
{
	checkf(Volume >= 0 && Volume <= 1, TEXT("Volume must be in range [0, 1]"));
	checkf(MusicSoundClass != nullptr, TEXT("MusicSoundClass should be specified"))
	Settings->MusicVolume = Volume;
	MusicSoundClass->Properties.Volume = Volume;
}

float UMyGameInstance::GetMusicVolume()
{
	return Settings->MusicVolume;
}

void UMyGameInstance::SetGameSoundVolume(float Volume)
{
	checkf(Volume >= 0 && Volume <= 1, TEXT("Volume must be in range [0, 1]"));
	checkf(GameSoundSoundClass != nullptr, TEXT("GameSoundSoundClass should be specified"))
	Settings->GameSoundVolume = Volume;
	GameSoundSoundClass->Properties.Volume = Volume;
}

float UMyGameInstance::GetGameSoundVolume()
{
	return Settings->GameSoundVolume;
}

void UMyGameInstance::StopMusic()
{
	auto LevelState = GetLevelStateActor();
	if (LevelState != nullptr)
	{
		LevelState->StopMusic();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ALevelStateActor* UMyGameInstance::GetLevelStateActor()
{
	return Cast<ALevelStateActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelStateActor::StaticClass()));
}

void UMyGameInstance::PauseGame(bool Pause)
{
	if (Pause)
	{
		auto PlayerController = Cast<APlayerControllerBase>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APlayerControllerBase::StaticClass())
		);
		if (PlayerController == nullptr)
		{
			return;
		}

		GetWorld()->GetAuthGameMode()->SetPause(PlayerController);
	}
	else
	{
		GetWorld()->GetAuthGameMode()->ClearPause();
	}
}

void UMyGameInstance::StartNewGame()
{
	CurrentSave = InitNewGameSaveObject();
	if (CurrentSave == nullptr)
	{
		check(false);
		return;
	}
	StopMusic();
	LoadingOverlayCallbackType = ELoadingOverlayCallbackType::NewGame;
	LoadingOverlayWidget->AddToViewport();
	LoadingOverlayWidget->StopAllAnimations();
	LoadingOverlayWidget->PlayFadeInAnimation();
}

TArray<FDateTimeAndString> UMyGameInstance::GetAllSaveGameSlots()
{
	TArray<FDateTimeAndString> Ret;
	if (
		UMySaveGameList* SaveGameList = Cast<UMySaveGameList>(UGameplayStatics::LoadGameFromSlot(SaveGameListSlot, 0))
		; SaveGameList != nullptr
	)
	{
		Ret = SaveGameList->Slots;
	}
	return Ret;
}

void UMyGameInstance::SaveGame(const FString& Slot)
{
	if (CurrentSave == nullptr)
	{
		check(false);
		return;
	}

	CurrentSave->Update(GetWorld());

	auto RealSlot = DecoratedSaveSlotName(Slot);
	OnSavingGameStarted.Broadcast();
	FAsyncSaveGameToSlotDelegate OnGameSavedNonMulticast;
	OnGameSavedNonMulticast.BindUObject(this, &UMyGameInstance::OnGameSavedNonMulticastTriggered);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, RealSlot, 0, OnGameSavedNonMulticast);
}

void UMyGameInstance::MakeQuickSave()
{
	SaveGame(QuickSaveSlotName);
}

void UMyGameInstance::MakeQuickLoad()
{
	auto Saves = GetAllSaveGameSlots();
	if (Saves.ContainsByPredicate([this](const FDateTimeAndString& Data){ return Data.String == QuickSaveSlotName; }))
	{
		LoadGame(QuickSaveSlotName);
	}
}

void UMyGameInstance::LoadLastSave()
{
	auto Saves = GetAllSaveGameSlots();
	if (Saves.IsEmpty())
	{
		return;
	}

	FDateTime MaxDateTime = FDateTime::FromUnixTimestamp(0);
	FString LatestSlot;
	bool bFound = false;
	for (auto& [DateTime, Slot] : Saves)
	{
		if (DateTime > MaxDateTime)
		{
			MaxDateTime = DateTime;
			LatestSlot = Slot;
			bFound = true;
		}
	}

	if (bFound)
	{
		LoadGame(LatestSlot);
	}
}

void UMyGameInstance::DeleteSave(const FString& Slot)
{
	UGameplayStatics::DeleteGameInSlot(DecoratedSaveSlotName(Slot), 0);
	UMySaveGameList* SaveGameList = Cast<UMySaveGameList>(UGameplayStatics::LoadGameFromSlot(SaveGameListSlot, 0));
	if (SaveGameList == nullptr)
	{
		return;
	}
	auto Index =
		SaveGameList->Slots.FindLastByPredicate(
			[&Slot](const FDateTimeAndString& DateTimeAndString){ return DateTimeAndString.String == Slot; }
		)
	;
	if (Index == INDEX_NONE)
	{
		return;
	}
	SaveGameList->Slots.RemoveAt(Index);
	UGameplayStatics::SaveGameToSlot(SaveGameList, SaveGameListSlot, 0);
}

void UMyGameInstance::LoadGame(const FString& Slot)
{
	/* Loading process:
	*  - UMyGameInstance::LoadGame (we're here)
	*  - LoadingOverlayWidget->FadeInAnimationFinished => UMyGameInstance::OnLoadingOverlayFadeInAnimationFinished (start actual loading)
	*  - AsyncLoadGameFromSlot finished => UMyGameInstance::OnGameLoadedNonMulticastTriggered (start loading level)
	*  - level is loaded => AMyGameModeBase::InitGame => UMyGameInstance::OnLevelLoaded
	*/
	if (LoadingOverlayWidget == nullptr)
	{
		return;
	}
	check(LoadGameImplSlot.IsEmpty());
	StopMusic();
	LoadGameImplSlot = Slot;
	LoadingOverlayCallbackType = ELoadingOverlayCallbackType::LoadGame;
	if (LoadingOverlayWidget->GetParent() == nullptr)
	{
		LoadingOverlayWidget->AddToViewport();
	}
	PauseGame(true);
	LoadingOverlayWidget->StopAllAnimations();
	LoadingOverlayWidget->PlayFadeInAnimation();
}

void UMyGameInstance::OnLevelLoaded()
{
	if (CurrentSave == nullptr)
	{
		// obviously it's a PIE session started in some level other than Main Menu
		CurrentSave = InitNewGameSaveObject();
		check(CurrentSave != nullptr);
		CurrentSave->CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(GetWorld()));
		CurrentSave->Apply(GetWorld());
		return;
	}

	CurrentSave->Apply(GetWorld());

	if (LoadingOverlayWidget != nullptr)
	{
		LoadingOverlayWidget->AddToViewport();
		LoadingOverlayWidget->StopAllAnimations();
		LoadingOverlayWidget->PlayFadeOutAnimation();
	}

	PauseGame(false);

	OnLoadingGameFinished.Broadcast();
}

void UMyGameInstance::OnLoadingOverlayFadeInAnimationFinished()
{
	switch (LoadingOverlayCallbackType)
	{
	case ELoadingOverlayCallbackType::NewGame:
	{
		UGameplayStatics::OpenLevel(GetWorld(), StartLevelName);
		break;
	}
	case ELoadingOverlayCallbackType::SendPlayerToAnotherLevel:
	{
		UGameplayStatics::OpenLevel(GetWorld(), CurrentSave->CurrentLevelName);
		break;
	}
	case ELoadingOverlayCallbackType::LoadGame:
	{
		check(!LoadGameImplSlot.IsEmpty());
		auto RealSlot = DecoratedSaveSlotName(LoadGameImplSlot);
		LoadGameImplSlot.Empty();
		OnLoadingGameStarted.Broadcast();
		FAsyncLoadGameFromSlotDelegate OnGameLoadedNonMulticast;
		OnGameLoadedNonMulticast.BindUObject(this, &UMyGameInstance::OnGameLoadedNonMulticastTriggered);
		UGameplayStatics::AsyncLoadGameFromSlot(RealSlot, 0, OnGameLoadedNonMulticast);
		break;
	}
	default:
		check(false);
		return;
	}
}

void UMyGameInstance::OnLoadingOverlayFadeOutAnimationFinished()
{
	if (LoadingOverlayWidget != nullptr)
	{
		LoadingOverlayWidget->RemoveFromViewport();
	}
}

void UMyGameInstance::GameOver()
{
	PauseGame(true);
	QuitToMainMenuImpl(true);
}

void UMyGameInstance::QuitToMainMenu()
{
	QuitToMainMenuImpl(false);
}

void UMyGameInstance::QuitToMainMenuImpl(bool showGameOverText)
{
	StopMusic();
	if (showGameOverText)
	{
		MainMenuTransitionOverlayType = EMainMenuTransitionOverlayType::GameOver;
		check(GameOverOverlayWidget != nullptr);
		if (GameOverOverlayWidget->GetParent() == nullptr)
		{
			GameOverOverlayWidget->AddToViewport();
		}
		GameOverOverlayWidget->StopAllAnimations();
		GameOverOverlayWidget->PlayFadeInAnimation();
	}
	else
	{
		MainMenuTransitionOverlayType = EMainMenuTransitionOverlayType::Regular;
		check(BlackScreenOverlayWidget != nullptr);
		if (BlackScreenOverlayWidget->GetParent() == nullptr)
		{
			BlackScreenOverlayWidget->AddToViewport();
		}
		BlackScreenOverlayWidget->StopAllAnimations();
		BlackScreenOverlayWidget->PlayFadeInAnimation();
	}
}

void UMyGameInstance::OnMainMenuLoaded()
{
	if (MainMenuTransitionOverlayType == EMainMenuTransitionOverlayType::GameOver)
	{
		if (GameOverOverlayWidget != nullptr)
		{
			GameOverOverlayWidget->AddToViewport();
			GameOverOverlayWidget->StopAllAnimations();
			GameOverOverlayWidget->PlayFadeOutAnimation();
		}
	}
	else
	{
		if (BlackScreenOverlayWidget != nullptr)
		{
			BlackScreenOverlayWidget->AddToViewport();
			BlackScreenOverlayWidget->StopAllAnimations();
			BlackScreenOverlayWidget->PlayFadeOutAnimation();
		}
	}

	PauseGame(false);
}

void UMyGameInstance::OnGameOverOverlayFadeInAnimationFinished()
{
	CurrentSave = nullptr;
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}

void UMyGameInstance::OnGameOverOverlayFadeOutAnimationFinished()
{
	if (GameOverOverlayWidget != nullptr)
	{
		GameOverOverlayWidget->RemoveFromViewport();
	}
}

void UMyGameInstance::OnBlackScreenOverlayFadeInAnimationFinished()
{
	OnGameOverOverlayFadeInAnimationFinished();
}

void UMyGameInstance::OnBlackScreenOverlayFadeOutAnimationFinished()
{
	if (BlackScreenOverlayWidget != nullptr)
	{
		BlackScreenOverlayWidget->RemoveFromViewport();
	}
}

FString UMyGameInstance::DecoratedSaveSlotName(FString SaveSlotName)
{
	return FString(SaveSlotNameDecoration) + SaveSlotName;
}

FString UMyGameInstance::NonDecoratedSaveSlotName(FString DecoratedSaveSlotName)
{
	return DecoratedSaveSlotName.Mid(sizeof(SaveSlotNameDecoration) - 1);
}

void UMyGameInstance::OnGameSavedNonMulticastTriggered(const FString& RealSlotName, const int32 UserIndex, bool succeeded)
{
	auto Slot = NonDecoratedSaveSlotName(RealSlotName);
	if (succeeded)
	{
		if (
			UMySaveGameList* SaveGameList = Cast<UMySaveGameList>(UGameplayStatics::LoadGameFromSlot(SaveGameListSlot, 0))
			; SaveGameList != nullptr
		)
		{
			auto Index =
				SaveGameList->Slots.FindLastByPredicate(
					[&Slot](const FDateTimeAndString& DateTimeAndString) { return DateTimeAndString.String == Slot; }
				)
			;
			if (Index != INDEX_NONE)
			{
				SaveGameList->Slots.RemoveAt(Index);
			}
			SaveGameList->Slots.Insert(FDateTimeAndString{FDateTime::UtcNow(), Slot}, 0);
			UGameplayStatics::SaveGameToSlot(SaveGameList, SaveGameListSlot, UserIndex);
		}
	}
	OnSavingGameFinished.Broadcast(Slot, UserIndex, succeeded);
}

void UMyGameInstance::OnGameLoadedNonMulticastTriggered(const FString& RealSlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	auto Slot = NonDecoratedSaveSlotName(RealSlotName);
	auto MySaveGame = Cast<UMySaveGame>(SaveGame);
	if (MySaveGame == nullptr)
	{
		check(false);
		return;
	}
	CurrentSave = MySaveGame;
	UGameplayStatics::OpenLevel(GetWorld(), CurrentSave->CurrentLevelName);
}

class UMySaveGame* UMyGameInstance::InitNewGameSaveObject()
{
	UMySaveGame* SaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	SaveGame->CurrentLevelName = StartLevelName;

	FGlobalActorSaveData GlobalActorSaveData;
	GlobalActorSaveData.Class = PlayerCharacterClass;
	SaveGame->GlobalActorSaveDatas.Add(FName("PlayerCharacter"), std::move(GlobalActorSaveData));

	return SaveGame;
}

void UMyGameInstance::RemoveTransformFromSaveGameForGlobalActor(FName GlobalActorName, FName LevelName)
{
	check(CurrentSave != nullptr);
	auto GlobalActorSaveData = CurrentSave->GlobalActorSaveDatas.Find(GlobalActorName);
	check(GlobalActorSaveData != nullptr);
	GlobalActorSaveData->Transforms.Remove(LevelName);
}

void UMyGameInstance::SendPlayerToPlayerStart(FName LevelName, FName PlayerStartName)
{
	if (GetWorld() == nullptr || LoadingOverlayWidget == nullptr)
	{
		return;
	}

	auto Character = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (Character == nullptr)
	{
		return;
	}

	if (LevelName.ToString() == UGameplayStatics::GetCurrentLevelName(GetWorld()))
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
		AActor** ActorPtr = Actors.FindByPredicate(
			[PlayerStartName](const AActor* Actor){ return Actor->GetFName() == PlayerStartName; }
		);
		if (ActorPtr == nullptr || *ActorPtr == nullptr)
		{
			return;
		}

		Character->SetActorTransform((*ActorPtr)->GetActorTransform());
		return;
	}

	StopMusic();
	PauseGame(true);

	LoadingOverlayWidget->AddToViewport();

	RemoveTransformFromSaveGameForGlobalActor(Character->GetFName(), LevelName);
	Character->DesiredPlayerStartNames.FindOrAdd(LevelName) = PlayerStartName;
	CurrentSave->Update(GetWorld());
	CurrentSave->CurrentLevelName = LevelName;

	LoadingOverlayCallbackType = ELoadingOverlayCallbackType::SendPlayerToAnotherLevel;

	LoadingOverlayWidget->StopAllAnimations();
	LoadingOverlayWidget->PlayFadeInAnimation();
}
