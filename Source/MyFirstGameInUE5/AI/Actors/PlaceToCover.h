#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PlaceToCover.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APlaceToCover : public AActor
{
	GENERATED_BODY()
	
public:	
	APlaceToCover();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* CoverPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	bool NeedToCrouch = false;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cover")
	bool GetIsOccupied() const { return bOccupied; }

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void SetIsOccupied(bool Value) { bOccupied = Value; }

private:
	bool bOccupied = false;
};
