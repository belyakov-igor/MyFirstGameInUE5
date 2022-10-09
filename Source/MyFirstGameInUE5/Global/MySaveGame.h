#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "MySaveGame.generated.h"

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
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FName LevelName = NAME_None;
};
