#include "AICharacter.h"

#include "Controllers/AIControllerBase.h"
#include "Global/MyGameModeBase.h"

#include "GameFramework/CharacterMovementComponent.h"
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
	}

	Cravings.bWantsToAim = true;
}

void AAICharacter::Die()
{
	Super::Die();

	auto AIController = Cast<AAIControllerBase>(Controller);
	if (AIController == nullptr || AIController->BrainComponent == nullptr)
	{
		return;
	}

	AIController->BrainComponent->Cleanup();

	if (!IsThereAnotherLivingCharacterOfTheSameGroup())
	{
		auto GameMode = GetWorld() != nullptr ? Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode()) : nullptr;
		GameMode->LastAICharacterInGroupDied.Broadcast(AIController->GroupId);
	}
}

bool AAICharacter::IsThereAnotherLivingCharacterOfTheSameGroup() const
{
	auto AIController = Cast<AAIControllerBase>(Controller);
	if (AIController == nullptr)
	{
		check(false);
		return false;
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIControllerBase::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		auto AnotherController = Cast<AAIControllerBase>(Actor);
		if (
			AnotherController != nullptr
			&& Actor != AIController
			&& AnotherController->GroupId == AIController->GroupId
			&& Cast<AAICharacter>(AnotherController->GetPawn()) != nullptr
			&& !Cast<AAICharacter>(AnotherController->GetPawn())->IsDead()
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
