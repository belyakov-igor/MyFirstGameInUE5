#pragma once

#include "Global/MySaveGame.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AISpawnAndOrderActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAISpawnAndOrderActor : public AActor, public ISavable
{
	GENERATED_BODY()
	
public:	
	AAISpawnAndOrderActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* SpawnTriggerComponent;

	virtual void BeginPlay() override;

	// Properties for game saving
	UPROPERTY(SaveGame)
	TArray<float> SavedSpawnDelays; // negative means character already spawned

	UPROPERTY(SaveGame)
	bool bAlreadyTriggered = false;

	virtual TArray<uint8> GetActorSaveData() override;

private:
	void Spawn();
};
