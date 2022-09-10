#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AISpawnAndOrderActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAISpawnAndOrderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAISpawnAndOrderActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* SpawnTriggerComponent;

private:
	bool bAlreadyTriggered = false;
};
