#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FTransform Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TSubclassOf<AActor> Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<uint8> Data;
};

USTRUCT(BlueprintType)
struct FGlobalActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TMap<FName /*Level name*/, FTransform> Transforms;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TSubclassOf<AActor> Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TArray<uint8> Data;
};

USTRUCT(BlueprintType)
struct FLevelSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TMap<FName /*Actor name*/, FActorSaveData> ActorSaveDatas;
};

UCLASS()
class MYFIRSTGAMEINUE5_API UMySaveGameList : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FDateTimeAndString> Slots; // always sorted
};

UCLASS()
class MYFIRSTGAMEINUE5_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FName CurrentLevelName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TMap<FName /*Level name*/, FLevelSaveData> LevelSaveDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TMap<FName /*Actor name*/, FGlobalActorSaveData> GlobalActorSaveDatas;

	void Update(UWorld* World);
	void Apply(UWorld* World) const;
};

//////////////////////////////////////////////////////////////////////////////////////

UINTERFACE(MinimalAPI, Blueprintable)
class USavable : public UInterface
{
	GENERATED_BODY()
};

class ISavable
{
	GENERATED_BODY()

public:
	virtual bool IsGlobal() const { return false; }
	virtual bool NoNeedToSaveThisOne() const { return false; }

	virtual void SetDefaultTansform() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save")
	TArray<uint8> GetActorSaveData();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save")
	void ApplyActorSaveData(const TArray<uint8>& Data);


	virtual TArray<uint8> GetActorSaveData_Implementation();
	virtual void ApplyActorSaveData_Implementation(const TArray<uint8>& Data);
};


UCLASS()
class MYFIRSTGAMEINUE5_API ASavableActor : public AActor, public ISavable
{
	GENERATED_BODY()
};
