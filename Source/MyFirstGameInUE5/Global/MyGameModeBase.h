#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "MyGameModeBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	explicit AMyGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
