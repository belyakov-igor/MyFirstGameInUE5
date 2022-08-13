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
				EBodyPart BodyPart = EBodyPart::Other;
				if (HitResult.BoneName.ToString().Contains("head"))
				{
					BodyPart = EBodyPart::Head;
				}
				UDamageTakerComponent::InflictPenetrationDamage(HitResult.GetActor(), Damage, BodyPart);
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
