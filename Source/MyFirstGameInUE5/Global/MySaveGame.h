#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "MySaveGame.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FName LevelName = NAME_None;

	UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;
};
