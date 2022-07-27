#include "Weapons/Actors/BaseWeapon.h"

#include "GameFramework/Character.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

APlayerController* ABaseWeapon::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (Player == nullptr)
        return nullptr;

    return Player->GetController<APlayerController>();
}
