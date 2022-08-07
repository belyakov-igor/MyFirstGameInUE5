#include "Weapons/Actors/PistolWeapon.h"

#include "Characters/PlayerCharacterBase.h"

#include "Weapons/Components/AmmoComponent.h"
#include "Characters/PlayerCharacterBase.h"

APistolWeapon::APistolWeapon()
{
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>("AmmoComponent");
}

void APistolWeapon::BeginPlay()
{
	Super::BeginPlay();

	AmmoComponent->ClipIsEmpty.AddUObject(AmmoComponent, &UAmmoComponent::Reload);
}

void APistolWeapon::SwitchCharacterToAnimationSet() const
{
	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	check(Character != nullptr);
	Character->AnimationSet = EPlayerCharacterBaseAnimationSet::Pistol;
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
	Character->PlayAnimMontage(
		Character->GetCrouchCoef() > 0.5f
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
