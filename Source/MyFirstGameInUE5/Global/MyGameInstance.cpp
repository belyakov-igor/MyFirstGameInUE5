#include "MyGameInstance.h"

#include "Settings.h"
#include "Global/MySaveGame.h"

#include "Components/AudioComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundClass.h"

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

	OnGameSavedNonMulticast.BindUObject(this, &UMyGameInstance::OnGameSavedNonMulticastTriggered);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UMyGameInstance::StartNewGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), StartLevelName);
}

void UMyGameInstance::QuitToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
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
	UMySaveGame* SaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	if (SaveGame == nullptr)
	{
		return;
	}

	auto RealSlot = DecoratedSaveSlotName(Slot);
	SaveGame->LevelName = FName(UGameplayStatics::GetCurrentLevelName(GetWorld()));
	OnSavingGameStarted.Broadcast();
	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, RealSlot, 0, OnGameSavedNonMulticast);
}

void UMyGameInstance::MakeQuickSave()
{
	SaveGame(QuickSaveSlotName);
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
