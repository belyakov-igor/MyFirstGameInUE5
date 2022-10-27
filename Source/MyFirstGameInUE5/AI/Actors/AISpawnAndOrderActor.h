#pragma once

#include "Global/MySaveGame.h"
#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AISpawnAndOrderActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAISpawnAndOrderActor : public AActor, public ISavable, public ITriggerable
{
	GENERATED_BODY()
	
public:	
	AAISpawnAndOrderActor();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	virtual void Trigger_Implementation() override { Spawn(); }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene = nullptr;

	virtual void BeginPlay() override;

	// Properties for game saving
	UPROPERTY(SaveGame)
	TArray<float> SavedSpawnDelays; // negative means character is already spawned

	UPROPERTY(SaveGame)
	bool bAlreadyTriggered = false;

	virtual TArray<uint8> GetActorSaveData_Implementation() override;

private:
	void Spawn();
};
