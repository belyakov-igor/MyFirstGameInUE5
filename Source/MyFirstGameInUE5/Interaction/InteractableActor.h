#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InteractableActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AInteractableActor : public AActor
{
	GENERATED_BODY()

public:	
	AInteractableActor();

	/** return text to display in HUD when this interactable is in focus */
	virtual FText FocusText() { return FText::GetEmpty(); }

	virtual void Interact(class APawn* Pawn) {};

	UFUNCTION(BlueprintNativeEvent)
	void FocusIn();

	UFUNCTION(BlueprintNativeEvent)
	void FocusOut();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* PawnOverlapCollisionComponent = nullptr;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	void FocusIn_Implementation() {}
	void FocusOut_Implementation() {}

private:
	class UInteractingComponent* FindInteractingComponent(AActor* Actor);
};
