#include "Weapons/Actors/BaseWeapon.h"

#include "GameFramework/Character.h"
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

	check(Cast<ACharacter>(NewOwner) != nullptr);
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

	auto Character = Cast<ACharacter>(GetOwner());
	check(Character != nullptr);

	const auto Controller = Character->GetController<APlayerController>();
	bool PlayerControlled = Controller != nullptr;

	check(WeaponMesh != nullptr);
	FVector MuzzleLocation = GetMuzzleSocketTransform().GetLocation();

	FVector ViewLocation;
	FRotator ViewRotation;
	if (PlayerControlled)
	{
		Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else
	{
		ViewLocation = MuzzleLocation;
		ViewRotation = Character->GetControlRotation();
	}

	FVector Direction = ViewRotation.Vector();
	float BlindAreaSize = PlayerControlled ? FVector::DotProduct(MuzzleLocation - ViewLocation, Direction) : 0.f;

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

FTransform ABaseWeapon::GetMuzzleSocketTransform() const
{
	check(WeaponMesh != nullptr);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}
