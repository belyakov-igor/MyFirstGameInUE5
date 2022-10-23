#include "AI/Components/MyAIPerceptionComponent.h"

#include "Global/Utilities/Components/ClampedIntegerComponent.h"
#include "Controllers/MyPlayerState.h"
#include "Characters/CharacterBase.h"

#include "AIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

AActor* UMyAIPerceptionComponent::GetEnemy()
{
	TArray<AActor*> PerceivedActors;
    GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
    GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), PerceivedActors);
    if (PerceivedActors.Num() == 0)
    {
        return nullptr;
    }

    const auto Controller = Cast<AAIController>(GetOwner());
    if (Controller == nullptr)
    {
        return nullptr;
    }

    const auto Pawn = Controller->GetPawn();
    const auto PlayerState = Cast<AMyPlayerState>(Controller->PlayerState);
    if (Pawn == nullptr || PlayerState == nullptr)
    {
        return nullptr;
    }

    int LowestHealth = INT_MAX;
    AActor* BestEnemy = nullptr;

    for (const auto OtherActor : PerceivedActors)
    {
        auto PerceivedCharacter = Cast<ACharacterBase>(OtherActor);
        if (PerceivedCharacter == nullptr)
        {
            PerceivedCharacter = Cast<ACharacterBase>(OtherActor->GetOwner());
        }
        if (PerceivedCharacter == nullptr || PerceivedCharacter->IsDead() || PerceivedCharacter->GetController() == nullptr)
        {
            continue;
        }

        const auto PerceivedActorPlayerState = Cast<AMyPlayerState>(PerceivedCharacter->GetController()->PlayerState);
        const bool AreEnemies = PerceivedActorPlayerState->GetTeamID() != PlayerState->GetTeamID();
        if (!AreEnemies)
        {
            continue;
        }
        
        auto HealthComponent = Cast<UClampedIntegerComponent>(PerceivedCharacter->GetDefaultSubobjectByName(HealthComponentName));
        if (HealthComponent == nullptr)
        {
            continue;
        }
        if (HealthComponent->GetValue() < LowestHealth)
        {
            LowestHealth = HealthComponent->GetValue();
            BestEnemy = PerceivedCharacter;
        }
    }

    return BestEnemy;
}

