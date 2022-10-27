#pragma once

#include "Global/Utilities/MyUtilities.h"

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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category = "Interaction")
	FText FocusText();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(class APawn* Pawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsEnabled = true;

	UPROPERTY(BlueprintAssignable, Category = "Focus")
	FSignalMulticastDynamicSignature FocusIn;

	UPROPERTY(BlueprintAssignable, Category = "Focus")
	FSignalMulticastDynamicSignature FocusOut;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* PawnOverlapCollisionComponent = nullptr;

	virtual FText FocusText_Implementation() { return FText::GetEmpty(); }
	virtual void Interact_Implementation(class APawn* Pawn) {}

	virtual void BeginPlay() override;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	class UInteractingComponent* FindInteractingComponent(AActor* Actor);
	FTimerHandle TimerHandle;

	void CheckOverlappingActors();
};
