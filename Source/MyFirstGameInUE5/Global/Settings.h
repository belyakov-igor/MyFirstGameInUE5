#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Settings.generated.h"

UCLASS(Config = Game)
class MYFIRSTGAMEINUE5_API USettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	float MusicVolume = 1.f;

	UPROPERTY(Config)
	float GameSoundVolume = 1.f;
};
