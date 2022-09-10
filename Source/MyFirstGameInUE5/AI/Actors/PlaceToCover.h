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
	class USceneComponent* CoverPositionRange1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* CoverPositionRange2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	bool NeedToCrouch = false;



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cover")
	FVector NearestPosInCover(FVector ToWhat) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cover")
	FVector OrientationInPos(FVector Pos) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cover")
	FVector GetCenter() const;
};
