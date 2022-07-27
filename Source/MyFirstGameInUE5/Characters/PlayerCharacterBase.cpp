#include "PlayerCharacterBase.h"

#include "Utilities/Components/ClampedIntegerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"

#include "Components/TextRenderComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerCharacterBase, All, All);

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UClampedIntegerComponent>("HealthComponent");
	WeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>("WeaponManagerComponent");

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
	HealthTextComponent->SetupAttachment(CameraComponent);

	check(GetCharacterMovement() != nullptr);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacterBase::TakeAnyDamage);
	HealthComponent->ReachedMin.AddUObject(this, &APlayerCharacterBase::Die);
	HealthComponent->ValueChanged.AddLambda(
		[this](float NewHealth)
		{
			HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewHealth)));
		}
	);
	HealthComponent->SetValue(HealthComponent->Max);

	CapsuleUprightHalfHeightBackup = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	SpringArmSocketOffsetZBackup = SpringArmComponent->SocketOffset.Z;
	MeshZOffsetFromCapsuleLowestPointBackup = GetMesh()->GetRelativeLocation().Z + GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	SetCapsuleHalfHeight(0.f);
	SetVelocityAccordingToCrouch(0.f);
}

void APlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController != nullptr)
	{
		UseUprightCameraPitch(NewController, true);
	}
}

void APlayerCharacterBase::UnPossessed()
{
	Super::UnPossessed();

	RestoreBackupCameraPitch();
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrouchIfPossible();
	RunIfPossible();
	UpdateUprightToCrouchSmoothCameraAndCapsuleTransition(DeltaTime);
	StandUprightIfPossible();
	RotateSelfIfNeeded(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent != nullptr);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacterBase::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacterBase::LookRight);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnCrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnCrouchButtonReleased);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnRunButtonPressed);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnRunButtonReleased);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnJumpButtonPressed);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::BeginAttack);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Released, this, &APlayerCharacterBase::EndAttack);
}

bool APlayerCharacterBase::IsRunning() const
{
	return bIsRunning;
}

bool APlayerCharacterBase::IsInCrouchState() const
{
	return CrouchCoef > 0.f;
}

float APlayerCharacterBase::GetForwardMovementInput() const
{
	return FVector::DotProduct(GetLastMovementInputVector(), GetCapsuleComponent()->GetForwardVector());
}

float APlayerCharacterBase::GetRightMovementInput() const
{
	return FVector::DotProduct(GetLastMovementInputVector(), GetCapsuleComponent()->GetRightVector());
}

float APlayerCharacterBase::GetCrouchCoef() const
{
	return CrouchCoef;
}

void APlayerCharacterBase::MoveForward(float Amount)
{
	auto dir = CameraComponent->GetForwardVector();
	dir.Z = 0;
	dir.Normalize();
	AddMovementInput(dir, Amount);
}

void APlayerCharacterBase::MoveRight(float Amount)
{
	AddMovementInput(CameraComponent->GetRightVector(), Amount);
}

void APlayerCharacterBase::LookUp(float Amount)
{
	AddControllerPitchInput(Amount);
}

void APlayerCharacterBase::LookRight(float Amount)
{
	AddControllerYawInput(Amount);
}

void APlayerCharacterBase::OnCrouchButtonPressed()
{
	bWantsToCrouch = true;
}

void APlayerCharacterBase::OnCrouchButtonReleased()
{
	bWantsToCrouch = false;
}

void APlayerCharacterBase::OnRunButtonPressed()
{
	bWantsToRun = true;
}

void APlayerCharacterBase::OnRunButtonReleased()
{
	bWantsToRun = false;
}

void APlayerCharacterBase::OnJumpButtonPressed()
{
	if (!IsInCrouchState())
	{
		Jump();
	}
}

void APlayerCharacterBase::CrouchIfPossible()
{
	bool previous = bIsCrouching;
	bIsCrouching = bWantsToCrouch && !IsRunning() && !GetCharacterMovement()->IsFalling();
	if (bIsCrouching != previous)
	{
		if (bIsCrouching) ToCrouchState();
		else ToUprightState();
	}
}

void APlayerCharacterBase::RunIfPossible()
{
	bool previous = bIsRunning;
	bIsRunning =
		bWantsToRun
		&& !IsInCrouchState()
		&& !GetCharacterMovement()->IsFalling()
		&& GetCharacterMovement()->Velocity.SquaredLength() > 2500
		&& !WeaponManagerComponent->AttackIsBeingPerformed()
	;
	if (bIsRunning != previous)
	{
		GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeedCoef * DefaultJogSpeed : DefaultJogSpeed;
	}
}

void APlayerCharacterBase::RestoreBackupCameraPitch()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController != nullptr)
	{
		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager; PlayerCameraManager != nullptr)
		{
			PlayerCameraManager->ViewPitchMin = MinCameraPitchBackup;
			PlayerCameraManager->ViewPitchMax = MaxCameraPitchBackup;
		}
	}
}

void APlayerCharacterBase::UseUprightCameraPitch(AController* Controller_, bool bMakeBackup)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller_); PlayerController != nullptr)
	{
		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager; PlayerCameraManager != nullptr)
		{
			if (bMakeBackup)
			{
				MinCameraPitchBackup = PlayerCameraManager->ViewPitchMin;
				MaxCameraPitchBackup = PlayerCameraManager->ViewPitchMax;
			}

			PlayerCameraManager->ViewPitchMin = MinCameraPitch;
			PlayerCameraManager->ViewPitchMax = MaxCameraPitch;
		}
	}
}

void APlayerCharacterBase::UseCrouchCameraPitch(float coef)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController != nullptr)
	{
		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager; PlayerCameraManager != nullptr)
		{
			PlayerCameraManager->ViewPitchMin = FMath::Lerp(MinCameraPitch, MinCameraPitchCrouched, coef);
			PlayerCameraManager->ViewPitchMax = FMath::Lerp(MaxCameraPitch, MaxCameraPitchCrouched, coef);
		}
	}
}

void APlayerCharacterBase::SetSpringArmRelativeZ(float coef)
{
	SpringArmComponent->SocketOffset.Z = SpringArmSocketOffsetZBackup + FMath::Lerp(0, SpringArmCrouchSocketOffsetZOffset, coef);
}

void APlayerCharacterBase::SetCapsuleHalfHeight(float coef)
{
	float HalfHeight = FMath::Lerp(CapsuleUprightHalfHeightBackup, CapsuleCrouchHalfHeight, coef);
	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeight);
	auto location = GetMesh()->GetRelativeLocation();
	location.Z = -HalfHeight + MeshZOffsetFromCapsuleLowestPointBackup;
	GetMesh()->SetRelativeLocation(location);
}

void APlayerCharacterBase::SetVelocityAccordingToCrouch(float coef)
{
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(1, CrouchVelocityCoef, coef) * DefaultJogSpeed;
}

void APlayerCharacterBase::UpdateUprightToCrouchSmoothCameraAndCapsuleTransition(float DeltaTime)
{
	static constexpr auto CubicCurve =
		[](float t)
		{
			float ret = -2*t*t*t + 3*t*t;
			return ret;
		}
	;

	if (CrouchSequenceType == ECrouchSequenceType::NONE)
	{
		return;
	}

	TimeInTransition += DeltaTime;
	float t = CubicCurve(FMath::Clamp(TimeInTransition / TransitionTime, 0.f, 1.f));
	float a = static_cast<int>(CrouchSequenceType);

	CrouchCoef = a * t + 0.5 * (1 - a);

	SetSpringArmRelativeZ(CrouchCoef);
	SetCapsuleHalfHeight(CrouchCoef);
	SetVelocityAccordingToCrouch(CrouchCoef);
	UseCrouchCameraPitch(CrouchCoef);

	if (TimeInTransition >= TransitionTime)
	{
		TimeInTransition = 0.f;
		if (CrouchSequenceType == ECrouchSequenceType::CROUCH_TO_UPRIGHT)
		{
			CrouchCoef = 0.f;
			bWantsToStandUpright = false;
			UseUprightCameraPitch(Controller, false);
		}
		CrouchSequenceType = ECrouchSequenceType::NONE;
	}
}

void APlayerCharacterBase::ToCrouchState()
{
	bWantsToStandUpright = false;
	if (CrouchSequenceType == ECrouchSequenceType::CROUCH_TO_UPRIGHT)
	{
		TimeInTransition = TransitionTime - TimeInTransition;
	}
	CrouchSequenceType = ECrouchSequenceType::UPRIGHT_TO_CROUCH;
}

void APlayerCharacterBase::ToUprightState()
{
	bWantsToStandUpright = true;
	if (!IsInCrouchState() || !CanStandUpright())
	{
		return;
	}
	if (CrouchSequenceType == ECrouchSequenceType::UPRIGHT_TO_CROUCH)
	{
		TimeInTransition = TransitionTime - TimeInTransition;
	}
	CrouchSequenceType = ECrouchSequenceType::CROUCH_TO_UPRIGHT;
}

bool APlayerCharacterBase::CanStandUpright() const
{
	if (!IsInCrouchState())
	{
		return true;
	}
	auto R = GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	auto HalfHeightCylinder = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight_WithoutHemisphere();
	auto P1 = GetCapsuleComponent()->GetComponentLocation();
	P1.Z += HalfHeightCylinder;
	auto L = 2 * (CapsuleUprightHalfHeightBackup - R - HalfHeightCylinder);
	auto P2 = P1 + GetCapsuleComponent()->GetUpVector() * L;
	FHitResult Result;
	auto bHit = UKismetSystemLibrary::SphereTraceSingle
	(
		/* WorldContextObject */ GetWorld()
		, /* Start */ P1
		, /* End */ P2
		, /* Radius */ R
		, /* TraceChannel */ UEngineTypes::ConvertToTraceType(ECC_WorldStatic)
		, /* bTraceComplex */ false
		, /* ActorsToIgnore */ TArray<AActor*>{GetCapsuleComponent()->GetOwner()}
		, /* DrawDebugType */ EDrawDebugTrace::Type::None //ForOneFrame
		, /* OutHit */ Result
		, /* bIgnoreSelf */ true
		, /* TraceColor */ FLinearColor(0.0f, 1.0f, 0.0f)
		, /* TraceHitColor */ FLinearColor(1.0f, 0.0f, 0.0f)
		, /* DrawTime */ 0.1f
	);
	return !bHit;
}

void APlayerCharacterBase::StandUprightIfPossible()
{
	if (IsInCrouchState() && bWantsToStandUpright && CanStandUpright())
	{
		CrouchSequenceType = ECrouchSequenceType::CROUCH_TO_UPRIGHT;
	}
}

void APlayerCharacterBase::BeginAttack()
{
	if (!bIsRunning && !GetCharacterMovement()->IsFalling())
	{
		WeaponManagerComponent->BeginAttack();
	}
}

void APlayerCharacterBase::EndAttack()
{
	if (!bIsRunning && !GetCharacterMovement()->IsFalling())
	{
		WeaponManagerComponent->EndAttack();
	}
}

void APlayerCharacterBase::Landed(const FHitResult& Hit)
{
	auto VelocityZ = -GetVelocity().Z;
	if (VelocityZ < LandingVelocityDamageRange.X)
	{
		return;
	}

	TakeDamage(
		FMath::GetMappedRangeValueClamped(LandingVelocityDamageRange, LandingDamageRange, VelocityZ)
		, FDamageEvent{}
		, nullptr
		, nullptr
	);
}

void APlayerCharacterBase::TakeAnyDamage(
	AActor* DamagedActor
	, float Damage
	, const class UDamageType* DamageType
	, class AController* InstigatedBy
	, AActor* DamageCauser
)
{
	HealthComponent->Decrease(static_cast<int32>(Damage));
	UE_LOG(PlayerCharacterBase, Display, TEXT("Damage taken: %d, Health: %d"), static_cast<int32>(Damage), HealthComponent->GetValue());
}

void APlayerCharacterBase::Die()
{
	PlayAnimMontage(DeathAnimMontage);
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponManagerComponent->EndAttack();
}

void APlayerCharacterBase::RotateSelfIfNeeded(float DeltaTime)
{
	if (!bSmoothlyOrientSelf_Required)
	{
		return;
	}
	auto CurrentRotation = GetActorRotation();
	auto TargetRotation = FRotator(CurrentRotation.Pitch, SmoothlyOrientSelf_WorldYawValue, CurrentRotation.Roll);
	auto ResultRotation = FMath::RInterpTo(
		CurrentRotation, TargetRotation, DeltaTime, InterpSpeedToSmoothlyOrientSelf
	);
	SetActorRotation(ResultRotation);
	if (FMath::IsNearlyZero(GetActorRotation().Yaw - SmoothlyOrientSelf_WorldYawValue, 0.01))
	{
		StopSmoothlyOrientSelfToWorldYawValue();
	}
}

void APlayerCharacterBase::SmoothlyOrientSelfToWorldYawValue(float WorldYawValue)
{
	bSmoothlyOrientSelf_Required = true;
	SmoothlyOrientSelf_WorldYawValue = WorldYawValue;
}

void APlayerCharacterBase::StopSmoothlyOrientSelfToWorldYawValue()
{
	bSmoothlyOrientSelf_Required = false;
}
