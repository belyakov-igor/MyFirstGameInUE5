#include "AICharacter.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"

AAICharacter::AAICharacter()
{
	bUseControllerRotationYaw = false;
	if (auto Movement = GetCharacterMovement(); Movement != nullptr)
	{
		Movement->bUseControllerDesiredRotation = true;
		Movement->RotationRate = FRotator(0, 200, 0);
		Movement->bUseRVOAvoidance = true;
	}

	Cravings.bWantsToAim = true;
}

void AAICharacter::Die()
{
	Super::Die();

	auto AIController = Cast<AAIController>(Controller);
	if (AIController == nullptr || AIController->BrainComponent == nullptr)
	{
		return;
	}

	AIController->BrainComponent->Cleanup();
}

void AAICharacter::Crouch(bool Crouch)
{
	Cravings.bWantsToCrouch = Crouch;
}
