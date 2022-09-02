#include "Weapons/Actors/BaseWeapon.h"

#include "Characters/PlayerCharacterBase.h"
#include "Weapons/Components/AmmoComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void ABaseWeapon::AttachToOwner(ACharacter* NewOwner)
{
	static const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);

	check(Cast<APlayerCharacterBase>(NewOwner) != nullptr);
	check(RootComponent != nullptr);
	SetOwner(NewOwner);
	auto f = RootComponent->AttachToComponent(GetCharacterMesh(), AttachmentRules, HandGripSocketName);
}

FWeaponUIData ABaseWeapon::GetUIData() const
{
	if (auto AmmoComponent = Cast<UAmmoComponent>(FindComponentByClass(UAmmoComponent::StaticClass())); AmmoComponent != nullptr)
	{
		return FWeaponUIData{WeaponIcon, CrossHairIcon, true, AmmoComponent->GetArsenalAmount(), AmmoComponent->GetClipAmount()};
	}
	return FWeaponUIData{WeaponIcon, CrossHairIcon, false, 0, 0};
}

APlayerController* ABaseWeapon::GetPlayerController() const
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (Player == nullptr)
		return nullptr;

	return Player->GetController<APlayerController>();
}

USkeletalMeshComponent* ABaseWeapon::GetCharacterMesh() const
{
	auto Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		return nullptr;
	}
	return Character->GetMesh();
}

std::pair<ACharacter*, USkeletalMeshComponent*> ABaseWeapon::GetCharacterAndCharacterMesh() const
{
	auto Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		return { nullptr, nullptr };
	}
	return { Character, Character->GetMesh() };
}

FHitResult ABaseWeapon::MakeTrace() const
{
	FHitResult HitResult;

	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	check(Character != nullptr);
	
	const auto Controller = Character->GetController<APlayerController>();
	if (!Controller)
	{
		return HitResult;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FVector Direction = ViewRotation.Vector();
	FVector MuzzleLocation = WeaponMesh->GetSocketTransform(MuzzleSocketName).GetLocation();
	float BlindAreaSize = FVector::DotProduct(MuzzleLocation - ViewLocation, Direction);

	if (BulletSpread > 0.f)
	{
		Direction = FMath::VRandCone(Direction, FMath::DegreesToRadians(BulletSpread));
	}

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	FVector Start = ViewLocation + Direction * BlindAreaSize;
	FVector End = ViewLocation + Direction * MaxShotDistance;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionQueryParams);

	End = HitResult.bBlockingHit ? HitResult.ImpactPoint : End;

	return HitResult;
}
