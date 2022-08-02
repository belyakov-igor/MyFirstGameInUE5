#include "Weapons/Actors/BaseWeapon.h"

#include "Characters/PlayerCharacterBase.h"

#include "DrawDebugHelpers.h"

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

	FVector Start = ViewLocation + Direction * BlindAreaSize;
	FVector End = ViewLocation + Direction * MaxShotDistance;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionQueryParams);

	End = HitResult.bBlockingHit ? HitResult.ImpactPoint : End;
	DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 5.f, 0, 2.f);

	return HitResult;
}
