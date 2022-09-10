#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"

#include "AIOrderMoveTargetComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UAIOrderMoveTargetComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveTarget")
	float TimeToStayInPosition = 0.f;
};
