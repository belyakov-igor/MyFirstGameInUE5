#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"

#include "MyAIPerceptionComponent.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UMyAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:
    AActor* GetEnemy();
};
