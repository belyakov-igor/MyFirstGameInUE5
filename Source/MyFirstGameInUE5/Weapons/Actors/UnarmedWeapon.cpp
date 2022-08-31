#include "Weapons/Actors/UnarmedWeapon.h"

#include "Animation/UnarmedHitAnimNotify.h"
#include "Animation/AttackEndedAnimNotify.h"
#include "Characters/PlayerCharacterBase.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"

void AUnarmedWeapon::BeginAttack()
{
	auto Character = Cast<ACharacter>(GetOwner());
	if (AttackAnimMontage == nullptr || Character == nullptr)
	{
		checkNoEntry();
		return;
	}
	Character->PlayAnimMontage(AttackAnimMontage, 1.f, AnimMontageSectionName);
	bAttackIsBeingPerformed = true;
	Character->GetCharacterMovement()->MovementMode = MOVE_None;
	Character->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
}

void AUnarmedWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitAnimations();
}

void AUnarmedWeapon::InitAnimations()
{
	auto HitNotify = UMyUtilities::FindNotifyByClass<UUnarmedHitAnimNotify>(AttackAnimMontage);
	auto AttackFinishedNotify = UMyUtilities::FindNotifyByClass<UAttackEndedAnimNotify>(AttackAnimMontage);
	if (HitNotify == nullptr || AttackFinishedNotify == nullptr)
	{
		checkf(false, TEXT("Attack finished notify or hit notify is missing on anim montage %s"), *AttackAnimMontage->GetName());
		return;
	}
	HitNotify->OnNotified.AddUObject(this, &AUnarmedWeapon::OnHitNotify);
	AttackFinishedNotify->OnNotified.AddUObject(this, &AUnarmedWeapon::OnAttackFinishedNotify);
}

FVector AUnarmedWeapon::GetHitLocation() const
{
	auto Mesh = GetCharacterMesh();
	if (Mesh == nullptr)
	{
		checkNoEntry();
		return {};
	}
	return Mesh->GetSocketTransform(UnarmedHitSocketName).GetLocation();
}

void AUnarmedWeapon::OnHitNotify(USkeletalMeshComponent* Mesh)
{
	auto [Character, CharacterMesh] = GetCharacterAndCharacterMesh();
	if (Mesh != CharacterMesh)
	{
		return;
	}
	auto ForwardVector = Character->GetCapsuleComponent()->GetForwardVector();
	auto CapsuleBegin = Character->GetCapsuleComponent()->GetComponentLocation();
	auto CapsuleEnd = GetHitLocation();
	CapsuleEnd = CapsuleBegin + FVector::DotProduct(CapsuleEnd - CapsuleBegin, ForwardVector) * ForwardVector;
	FHitResult HitResult;
	if (
		!UKismetSystemLibrary::SphereTraceSingleForObjects
		(
			/*WorldContextObject*/ GetWorld()
			, /*Start*/ CapsuleBegin
			, /*End*/ CapsuleEnd
			, /*Radius*/ TraceSphereRadius
			, /*ObjectTypes*/ TArray{ TEnumAsByte(UEngineTypes::ConvertToObjectType(ECC_Pawn)) }
			, /*bTraceComplex*/ false
			, /*ActorsToIgnore*/{ Character }
			, /*DrawDebugType*/ EDrawDebugTrace::Type::None //ForDuration
			, /*OutHit*/ HitResult
			, /*bIgnoreSelf*/ true
			, /*TraceColor*/ FLinearColor::Green
			, /*TraceHitColor*/ FLinearColor::Red
			, /*DrawTime*/ 3.f
		)
	)
	{
		return;
	}


	UDamageTakerComponent::InflictDamage(HitResult.GetActor(), HitResult.BoneName, Damage);
	UDamageTakerComponent::GiveMomentum(
		HitResult.GetActor()
		, HitResult.BoneName
		, HitResult.ImpactPoint
		, ForwardVector * PunchMomentum
	);
}

void AUnarmedWeapon::OnAttackFinishedNotify(USkeletalMeshComponent* Mesh)
{
	if (Mesh != GetCharacterMesh())
	{
		return;
	}

	auto Character = Cast<APlayerCharacterBase>(GetOwner());
	if (Character == nullptr)
	{
		checkNoEntry();
		return;
	}
	Character->GetCharacterMovement()->MovementMode = MOVE_Walking;
	bAttackIsBeingPerformed = false;
	OnAttackFinished.Execute();
}
