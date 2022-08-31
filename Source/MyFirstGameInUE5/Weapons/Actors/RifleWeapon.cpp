#include "Weapons/Actors/RifleWeapon.h"

#include "Weapons/Components/AmmoComponent.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

#include "GameFramework/Character.h"

ARifleWeapon::ARifleWeapon()
{
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>("AmmoComponent");
}

void ARifleWeapon::BeginPlay()
{
	Super::BeginPlay();

	AmmoComponent->ClipIsEmpty.AddUObject(AmmoComponent, &UAmmoComponent::Reload);
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
				auto Character = Cast<ACharacter>(GetOwner());
				if (Character == nullptr)
				{
					return;
				}
				Character->PlayAnimMontage(IsCrouching ? CrouchFireAnimMontage : UprightFireAnimMontage);
				AmmoComponent->DecreaseClip(1);
				FHitResult HitResult = MakeTrace();
				if (!HitResult.bBlockingHit)
				{
					return;
				}
				UDamageTakerComponent::InflictDamage(HitResult.GetActor(), HitResult.BoneName, Damage);
				UDamageTakerComponent::GiveMomentum(
					HitResult.GetActor()
					, HitResult.BoneName
					, HitResult.ImpactPoint
					, (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal()* BulletMomentum
				);
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
