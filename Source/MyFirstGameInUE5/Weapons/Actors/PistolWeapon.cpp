#include "Weapons/Actors/PistolWeapon.h"

#include "Characters/PlayerCharacterBase.h"

#include "Weapons/Components/AmmoComponent.h"
#include "Weapons/Components/StandardFirearmFXComponent.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

APistolWeapon::APistolWeapon()
{
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>("AmmoComponent");
	StandardFirearmFXComponent = CreateDefaultSubobject<UStandardFirearmFXComponent>("StandardFirearmFXComponent");
}

void APistolWeapon::BeginPlay()
{
	Super::BeginPlay();

	AmmoComponent->ClipIsEmpty.AddUObject(AmmoComponent, &UAmmoComponent::Reload);
}

void APistolWeapon::BeginAttack()
{
	if (bAttackIsBeingPerformed)
	{
		return;
	}
	if (AmmoComponent->GetClipAmount() == 0)
	{
		return;
	}
	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	if (Character == nullptr)
	{
		return;
	}
	bAttackIsBeingPerformed = true;
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle
		, [this]{ bAttackIsBeingPerformed = false; }
		, /*InRate*/ TimeBetweenShots
		, /*bInLoop*/ false
	);
	Character->PlayAnimMontage(IsCrouching ? CrouchFireAnimMontage : UprightFireAnimMontage);
	AmmoComponent->DecreaseClip(1);
	FHitResult HitResult = MakeTrace();
	StandardFirearmFXComponent->PlayFX(HitResult, WeaponMesh, MuzzleSocketName);
	if (!HitResult.bBlockingHit)
	{
		return;
	}
	UDamageTakerComponent::InflictDamage(HitResult.GetActor(), HitResult.BoneName, Damage);
	UDamageTakerComponent::GiveMomentum(
		HitResult.GetActor()
		, HitResult.BoneName
		, HitResult.ImpactPoint
		, (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal() * BulletMomentum
	);
}
