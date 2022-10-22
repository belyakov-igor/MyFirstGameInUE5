#include "Global/MySaveGame.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

TArray<uint8> ISavable::GetActorSaveData()
{
	auto Actor = Cast<AActor>(this);
	check(Actor != nullptr);
	TArray<uint8> Data;
	FMemoryWriter MemWriter(Data);
	FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
	Ar.ArIsSaveGame = true;
	Actor->Serialize(Ar);

	return Data;
}

void ISavable::ApplyActorSaveData(const TArray<uint8>& Data)
{
	auto Actor = Cast<AActor>(this);
	check(Actor != nullptr);
	FMemoryReader MemReader(Data);
	FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
	Ar.ArIsSaveGame = true;
	Actor->Serialize(Ar);
}

void UMySaveGame::Update(UWorld* World)
{
	check(World != nullptr);

	CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(World));

	auto& LevelSaveData = LevelSaveDatas.FindOrAdd(CurrentLevelName);
	LevelSaveData.ActorSaveDatas.Empty();
	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->Implements<USavable>())
		{
			continue;
		}

		auto Savable = Cast<ISavable>(Actor);
		check(Savable != nullptr);
		if (Savable->NoNeedToSaveThisOne())
		{
			continue;
		}

		if (Savable->IsGlobal())
		{
			auto& GlobalActorSaveData = GlobalActorSaveDatas.FindOrAdd(Actor->GetFName());
			GlobalActorSaveData.Transforms.FindOrAdd(CurrentLevelName) = Actor->GetActorTransform();
			GlobalActorSaveData.Class = Actor->GetClass();
			GlobalActorSaveData.Data = Savable->GetActorSaveData();
		}
		else
		{
			FActorSaveData ActorSaveData;
			ActorSaveData.Transform = Actor->GetActorTransform();
			ActorSaveData.Class = Actor->GetClass();
			ActorSaveData.Data = Savable->GetActorSaveData();
			LevelSaveData.ActorSaveDatas.Add(Actor->GetFName(), std::move(ActorSaveData));
		}
	}
}

void UMySaveGame::Apply(UWorld* World) const
{
	check(World != nullptr);
	check(CurrentLevelName == FName(UGameplayStatics::GetCurrentLevelName(World)));

	auto LevelSaveData = LevelSaveDatas.Find(CurrentLevelName);
	TSet<FName> ProcessedActorNames;
	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USavable>())
		{
			continue;
		}

		auto Savable = Cast<ISavable>(Actor);
		check(Savable != nullptr);

		if (Savable->IsGlobal())
		{
			auto GlobalActorSaveData = GlobalActorSaveDatas.Find(Actor->GetFName());
			if (GlobalActorSaveData != nullptr)
			{
				auto Transform = GlobalActorSaveData->Transforms.Find(CurrentLevelName);
				Actor->SetActorTransform(Transform != nullptr ? *Transform : Savable->GetDefaultTansform());
				Savable->ApplyActorSaveData(GlobalActorSaveData->Data);
				ProcessedActorNames.Add(Actor->GetFName());
			}
			else
			{
				Actor->Destroy();
			}
		}
		else if (LevelSaveData != nullptr)
		{
			const FActorSaveData* ActorSaveData = LevelSaveData->ActorSaveDatas.Find(Actor->GetFName());
			if (ActorSaveData != nullptr)
			{
				Actor->SetActorTransform(ActorSaveData->Transform);
				Savable->ApplyActorSaveData(ActorSaveData->Data);
				ProcessedActorNames.Add(Actor->GetFName());
			}
			else
			{
				Actor->Destroy();
			}
		}
	}

	static constexpr auto Spawn = [](UWorld* World, UClass* Class, const FTransform* Transform, FName Name, const TArray<uint8>& Data)
	{
		check(World != nullptr);
		FActorSpawnParameters Parameters;
		Parameters.Name = Name;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		auto Actor = World->SpawnActor(Class, Transform, Parameters);
		auto Savable = Cast<ISavable>(Actor);
		check(Savable != nullptr);
		if (Savable != nullptr && Transform == nullptr)
		{
			Actor->SetActorTransform(Savable->GetDefaultTansform());
		}
		Savable->ApplyActorSaveData(Data);
	};
	for (const auto& Pair : GlobalActorSaveDatas)
	{
		if (ProcessedActorNames.Find(Pair.Key) == nullptr)
		{
			Spawn(World, Pair.Value.Class, Pair.Value.Transforms.Find(CurrentLevelName), Pair.Key, Pair.Value.Data);
		}
	}
	if (LevelSaveData != nullptr)
	{
		for (const auto& Pair : LevelSaveData->ActorSaveDatas)
		{
			if (ProcessedActorNames.Find(Pair.Key) == nullptr)
			{
				Spawn(World, Pair.Value.Class, &Pair.Value.Transform, Pair.Key, Pair.Value.Data);
			}
		}
	}
}
