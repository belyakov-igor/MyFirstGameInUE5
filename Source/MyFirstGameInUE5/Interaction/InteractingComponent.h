#pragma once

#include "global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "InteractingComponent.generated.h"

class AInteractableActor;

UCLASS()
class MYFIRSTGAMEINUE5_API UInteractingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UInteractingComponent();

	void AddInteractableActor(AInteractableActor* InteractableActor);
	void RemoveInteractableActor(AInteractableActor* InteractableActor);
	void Interact();

	FTextMulticastSignature ChangeHUDText;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Set positive value to interact only with objecs in front of the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float MinDotProductForInteractableDirection = 0.2f;

	virtual void BeginPlay() override;

private:
	TArray<AInteractableActor*> InteractableActors;
	AInteractableActor* ActorInFocus = nullptr;
};
