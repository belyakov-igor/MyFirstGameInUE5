#include "Weapons/Actors/RifleWeapon.h"

#include "Weapons/Components/AmmoComponent.h"
#include "Characters/PlayerCharacterBase.h"

ARifleWeapon::ARifleWeapon()
{
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>("AmmoComponent");
}

void ARifleWeapon::BeginPlay()
{
	Super::BeginPlay();

	AmmoComponent->ClipIsEmpty.AddUObject(AmmoComponent, &UAmmoComponent::Reload);
}

void ARifleWeapon::SwitchCharacterToAnimationSet() const
{
	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	check(Character != nullptr);
	Character->AnimationSet = EPlayerCharacterBaseAnimationSet::Rifle;
}

void ARifleWeapon::BeginAttack()
{
	if (bAttackIsBeingPerformed)
	{
		return;
	}
	bAttackIsBeingPerformed = true; 
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle
		, [this]
			{
				if (AmmoComponent->GetClipAmount() == 0)
				{
					return;
				}
				auto Character = Cast<APlayerCharacterBase>(GetOwner());
				if (Character == nullptr)
				{
					return;
				}
				Character->PlayAnimMontage(
					Character->GetCrouchCoef() > 0.5
					? CrouchFireAnimMontage
					: UprightFireAnimMontage
				);
				AmmoComponent->DecreaseClip(1);
				FHitResult HitResult = MakeTrace();
				if (HitResult.bBlockingHit && HitResult.GetActor())
				{
					HitResult.GetActor()->TakeDamage(Damage, FDamageEvent{}, GetPlayerController(), this);
				}
			}
		, /*InRate*/ TimeBetweenShots
		, /*bInLoop*/ true
		, /*InFirstDelay*/ 0.f
	);
}

void ARifleWeapon::EndAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	bAttackIsBeingPerformed = false;
}
