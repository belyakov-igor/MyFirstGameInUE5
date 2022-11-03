#include "AICharacter.h"

#include "Global/MyGameModeBase.h"
#include "Global/MyGameInstance.h"
#include "Global/LevelStateActor.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"

AAICharacter::AAICharacter()
{
	bUseControllerRotationYaw = false;
	if (auto Movement = GetCharacterMovement(); Movement != nullptr)
	{
		Movement->bUseControllerDesiredRotation = true;
		Movement->RotationRate = FRotator(0, 200, 0);
		Movement->bUseRVOAvoidance = true;
		Movement->AvoidanceWeight = 1.f;
		Movement->SetGroupsToIgnore(0b10);
		Movement->SetAvoidanceGroup(0b1);
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

	if (auto Movement = GetCharacterMovement(); Movement != nullptr)
	{
		Movement->SetAvoidanceGroup(0b10);
	}

	if (
		auto LevelState = UMyGameInstance::GetMyGameInstance(GetWorld())->GetLevelStateActor();
		LevelState != nullptr
	)
	{
		LevelState->AICharacterDied.Broadcast();
	}
}

bool AAICharacter::IsThereAnyLivingAICharacter()
{
	auto World = GEngine->GameViewport->GetWorld();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		if (
			Cast<AAICharacter>(Actor) != nullptr
			&& !Cast<AAICharacter>(Actor)->IsDead()
		)
		{
			return true;
		}
	}
	return false;
}

void AAICharacter::Crouch(bool Crouch)
{
	Cravings.bWantsToCrouch = Crouch;
}
