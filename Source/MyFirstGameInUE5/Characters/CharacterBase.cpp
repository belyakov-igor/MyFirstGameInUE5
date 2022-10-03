#include "CharacterBase.h"

#include "Utilities/Components/ClampedIntegerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Actors/BaseWeapon.h"
#include "UI/CharacterManHUDWidget.h"
#include "Interaction/InteractingComponent.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

#include "Components/TextRenderComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UClampedIntegerComponent>(HealthComponentName);
	StaminaComponent = CreateDefaultSubobject<UClampedIntegerComponent>(StaminaComponentName);
	MeleeWeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>(MeleeWeaponManagerComponentName);
	MeleeWeaponManagerComponent->OnAttackFinished.BindUObject(this, &ACharacterBase::OnMeleeAttackFinished);
	RangedWeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>(RangedWeaponManagerComponentName);
	RangedWeaponManagerComponent->OnAttackFinished.BindUObject(this, &ACharacterBase::OnRangedAttackFinished);

	InteractingComponent = CreateDefaultSubobject<UInteractingComponent>("InteractingComponent");
	InteractingComponent->SetupAttachment(GetCapsuleComponent());

	DamageTakerComponent = CreateDefaultSubobject<UDamageTakerComponent>("DamageTakerComponent");

	check(GetCharacterMovement() != nullptr);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
	SetMovementSettingsForNotRunning();

	SetState(State_UprightNotRunning);
	SetAimState(State_Aim_NoAim);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	check(State != nullptr);
	check(AimState != nullptr);

	HealthComponent->ReachedMin.AddUObject(this, &ACharacterBase::Die);
	HealthComponent->SetValue(HealthComponent->Max);

	StaminaComponent->SetValue(StaminaComponent->Max);
	StaminaComponent->ValueChanged.AddUObject(this, &ACharacterBase::WaitForSomeTimeAndStartRegeneratingStaminaIfNeeded);

	RangedWeaponManagerComponent->OnWeaponAndAmmoChanged.BindUObject(this, &ACharacterBase::OnWeaponAndAmmoChanged);

	{
		checkf(DefaultMeleeWeaponClass != nullptr, TEXT("Default melee weapon should be specified."));
		auto DefaultMeleeWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultMeleeWeaponClass);
		MeleeWeaponManagerComponent->AddWeapon(DefaultMeleeWeapon);
	}

	CapsuleUprightHalfHeightBackup = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MeshZOffsetFromCapsuleLowestPointBackup = GetMesh()->GetRelativeLocation().Z + GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	UprightToCrouchUpdater.TransitionTime = UprightToCrouchTransitionTime;
	UprightToCrouchUpdater.Callback = [this](float Coef){ UprightToCrouchTransitionCallback(Coef); };
	UprightToCrouchUpdater.Callback(0.f);


	AimToNoAimUpdater.TransitionTime = AimToNoAimTransitionTime;
	AimToNoAimUpdater.Callback = [this](float Coef){ AimToNoAimTransitionCallback(Coef); };
	AimToNoAimUpdater.Callback(0.f);

	DamageTakerComponent->DamageTaken.AddDynamic(this, &ACharacterBase::TakeDamageCallback);
	DamageTakerComponent->MomentumTaken.AddDynamic(this, &ACharacterBase::TakeMomentumCallback);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	State->Tick(DeltaTime);
	AimState->Tick(DeltaTime);

	RotateSelfIfNeeded(DeltaTime);
}

bool ACharacterBase::IsInRunState() const
{
	return State == &State_UprightRunning;
}

bool ACharacterBase::IsInCrouchState() const
{
	return State == &State_Crouch;
}

float ACharacterBase::GetForwardMovementInput() const
{
	return FVector::DotProduct(GetLastMovementInputVector(), GetCapsuleComponent()->GetForwardVector());
}

float ACharacterBase::GetRightMovementInput() const
{
	return FVector::DotProduct(GetLastMovementInputVector(), GetCapsuleComponent()->GetRightVector());
}

float ACharacterBase::GetCrouchCoef() const
{
	return CrouchCoef;
}

bool ACharacterBase::AttackIsBeingPerformed() const
{
	return MeleeWeaponManagerComponent->AttackIsBeingPerformed() || RangedWeaponManagerComponent->AttackIsBeingPerformed();
}

void ACharacterBase::PlayFootstepSound() const
{
	auto TraceStart = GetActorLocation();
	auto TraceEnd = TraceStart;
	TraceStart.Z -= GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight_WithoutHemisphere();
	TraceEnd.Z -= GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 1.2f;
	FHitResult HitResult;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionQueryParams);
	if (!HitResult.bBlockingHit)
	{
		return;
	}

	auto FootstepSoundPtr = SurfaceFootstepSoundMap.Find(HitResult.PhysMaterial.Get());
	if (FootstepSoundPtr == nullptr)
	{
		FootstepSoundPtr = &DefaultFootstepSound;
	}
	auto FootstepSound = *FootstepSoundPtr;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSound, HitResult.ImpactPoint);
}

bool ACharacterBase::RunningIsPossible() const
{
	return
		Cravings.bWantsToRun
		&& AimState == &State_Aim_NoAim
		&& !GetCharacterMovement()->IsFalling()
		&& GetCharacterMovement()->Velocity.SquaredLength() > 2500
		&& !MeleeWeaponManagerComponent->AttackIsBeingPerformed()
		&& !RangedWeaponManagerComponent->AttackIsBeingPerformed()
		&& StaminaComponent->GetValue() > 0
	;
}


// FState_UprightNotRunning { ---------------------------------------------------------------------------

void ACharacterBase::FState_UprightNotRunning::Tick(float DeltaTime)
{
	if (Character.Cravings.bWantsToCrouch && !Character.GetCharacterMovement()->IsFalling())
	{
		Character.SetState(Character.State_TransitionUprightNotRunningToCrouch);
	}

	if (Character.RunningIsPossible())
	{
		Character.SetState(Character.State_UprightRunning);
	}
}

void ACharacterBase::FState_UprightNotRunning::TakeOver()
{
	Character.GetCharacterMovement()->MaxWalkSpeed = Character.DefaultJogSpeed;
}

// } FState_UprightNotRunning ---------------------------------------------------------------------------


// FState_UprightRunning { ------------------------------------------------------------------------------

void ACharacterBase::FState_UprightRunning::Tick(float DeltaTime)
{
	if (!Character.RunningIsPossible())
	{
		Character.GetWorld()->GetTimerManager().ClearTimer(Character.StaminaDecreasingTimerHandle);
		Character.SetMovementSettingsForNotRunning();
		Character.SetState(Character.State_UprightNotRunning);
	}
}

void ACharacterBase::FState_UprightRunning::TakeOver()
{
	Character.GetWorld()->GetTimerManager().SetTimer(
		Character.StaminaDecreasingTimerHandle
		, [this] { Character.StaminaComponent->Decrease(Character.AmountOfStaminaDecresingOnRunning); }
		, /*InRate*/ Character.TimeIntervalForStaminaDecresingOnRunning
		, /*bInLoop*/ true
	);
	Character.GetCharacterMovement()->MaxWalkSpeed = Character.RunSpeedCoef * Character.DefaultJogSpeed;
	Character.SetMovementSettingsForRunning();
}

// } FState_UprightRunning ------------------------------------------------------------------------------


// FState_Crouch { --------------------------------------------------------------------------------------

void ACharacterBase::FState_Crouch::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToCrouch && Character.CanStandUpright())
	{
		Character.SetState(Character.State_TransitionCrouchToUprightNotRunning);
	}
}

void ACharacterBase::FState_Crouch::TakeOver()
{
}

// } FState_Crouch --------------------------------------------------------------------------------------


// FState_TransitionUprightNotRunningToCrouch { ---------------------------------------------------------

void ACharacterBase::FState_TransitionUprightNotRunningToCrouch::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToCrouch && Character.CanStandUpright())
	{
		Character.SetState(Character.State_TransitionCrouchToUprightNotRunning);
		return;
	}
	if (Character.UprightToCrouchUpdater.Update(DeltaTime) == ETransitionFinished::Yes)
	{
		Character.SetCapsuleHalfHeight(1.f);
		Character.SetState(Character.State_Crouch);
	}
}

void ACharacterBase::FState_TransitionUprightNotRunningToCrouch::TakeOver()
{
}

// } FState_TransitionUprightNotRunningToCrouch ---------------------------------------------------------


// FState_TransitionCrouchToUprightNotRunning { ---------------------------------------------------------

void ACharacterBase::FState_TransitionCrouchToUprightNotRunning::Tick(float DeltaTime)
{
	if (Character.Cravings.bWantsToCrouch)
	{
		Character.SetState(Character.State_TransitionUprightNotRunningToCrouch);
		return;
	}
	if (Character.UprightToCrouchUpdater.Update(-DeltaTime) == ETransitionFinished::Yes)
	{
		Character.CrouchCoef = 0.f;
		Character.SetState(Character.State_UprightNotRunning);
	}
}

void ACharacterBase::FState_TransitionCrouchToUprightNotRunning::TakeOver()
{
	Character.SetCapsuleHalfHeight(0.f);
}

// } FState_TransitionCrouchToUprightNotRunning ---------------------------------------------------------




// FState_Aim_NoAim { -----------------------------------------------------------------------------------

void ACharacterBase::FState_Aim_NoAim::Tick(float DeltaTime)
{
	if (
		Character.Cravings.bWantsToAim
		&& Character.RangedWeaponManagerComponent->GetCurrentWeapon() != nullptr
		&& !Character.MeleeWeaponManagerComponent->AttackIsBeingPerformed()
	)
	{
		Character.SetAimState(Character.State_Aim_TransitionNoAimToAim);
	}
}

void ACharacterBase::FState_Aim_NoAim::TakeOver()
{
	Character.GetCharacterMovement()->bOrientRotationToMovement = true;
	Character.GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

// } FState_Aim_NoAim -----------------------------------------------------------------------------------


// FState_Aim_Aim { -------------------------------------------------------------------------------------

void ACharacterBase::FState_Aim_Aim::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToAim)
	{
		Character.SetAimState(Character.State_Aim_TransitionAimToNoAim);
	}
}

void ACharacterBase::FState_Aim_Aim::TakeOver()
{
	Character.GetCharacterMovement()->bOrientRotationToMovement = false;
	Character.GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

// } FState_Aim_Aim -------------------------------------------------------------------------------------


// FState_Aim_TransitionNoAimToAim { --------------------------------------------------------------------

void ACharacterBase::FState_Aim_TransitionNoAimToAim::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToAim)
	{
		Character.SetAimState(Character.State_Aim_TransitionAimToNoAim);
		return;
	}
	if (Character.AimToNoAimUpdater.Update(DeltaTime) == ETransitionFinished::Yes)
	{
		Character.SetAimState(Character.State_Aim_Aim);
	}
}

void ACharacterBase::FState_Aim_TransitionNoAimToAim::TakeOver()
{
	Character.AimRotationCurrent = Character.GetCapsuleComponent()->GetComponentRotation();
	Character.RangedWeaponManagerComponent->SetWeaponVisibility(true);
	auto Weapon = Character.RangedWeaponManagerComponent->GetCurrentWeapon();
	check(Weapon != nullptr);
	Character.AnimationSet = Weapon->GetCharacterAnimationSet();
}

// } FState_Aim_TransitionNoAimToAim --------------------------------------------------------------------


// FState_Aim_TransitionAimToNoAim { --------------------------------------------------------------------

void ACharacterBase::FState_Aim_TransitionAimToNoAim::Tick(float DeltaTime)
{
	if (Character.Cravings.bWantsToAim)
	{
		Character.SetAimState(Character.State_Aim_TransitionNoAimToAim);
		return;
	}
	if (Character.AimToNoAimUpdater.Update(-DeltaTime) == ETransitionFinished::Yes)
	{
		Character.RangedWeaponManagerComponent->SetWeaponVisibility(false);
		Character.AnimationSet = ECharacterAnimationSet::Unarmed;
		Character.SetAimState(Character.State_Aim_NoAim);
	}
}

void ACharacterBase::FState_Aim_TransitionAimToNoAim::TakeOver()
{
	auto Rotation = Character.GetControlRotation();
	Rotation.Pitch = 0.f;
	Character.AimRotationCurrent = Rotation;
	Character.RangedWeaponManagerComponent->EndAttack();
}

// } FState_Aim_TransitionAimToNoAim --------------------------------------------------------------------

void ACharacterBase::SetCapsuleHalfHeight(float coef)
{
	float HalfHeightOld = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeight = FMath::Lerp(CapsuleUprightHalfHeightBackup, CapsuleCrouchHalfHeight, coef);
	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeight);
	auto location = GetCapsuleComponent()->GetComponentLocation();
	location.Z -= HalfHeightOld - HalfHeight;
	GetCapsuleComponent()->SetWorldLocation(location);

	location = GetMesh()->GetRelativeLocation();
	location.Z = -HalfHeight + MeshZOffsetFromCapsuleLowestPointBackup;
	GetMesh()->SetRelativeLocation(location);

	if (auto SpringArmComponent = FindComponentByClass<USpringArmComponent>();  SpringArmComponent != nullptr)
	{
		location = SpringArmComponent->GetRelativeLocation();
		location.Z = CapsuleUprightHalfHeightBackup - HalfHeight;
		SpringArmComponent->SetRelativeLocation(location);
	}
}

void ACharacterBase::SetVelocityAccordingToCrouch(float coef)
{
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(1, CrouchVelocityCoef, coef) * DefaultJogSpeed;
}

void ACharacterBase::UprightToCrouchTransitionCallback(float Coef)
{
	if (
		(CrouchCoef < 0.5f && Coef >= 0.5f)
		|| (CrouchCoef >= 0.5f && Coef < 0.5f)
	)
	{
		StopAnimMontage(GetCurrentMontage());
		MeleeWeaponManagerComponent->SetIsCrouching(Coef >= 0.5f);
		RangedWeaponManagerComponent->SetIsCrouching(Coef >= 0.5f);
	}
	CrouchCoef = Coef;
	SetVelocityAccordingToCrouch(Coef);
}

void ACharacterBase::SetAimRotation(float coef)
{
	FRotator Rotation = GetControlRotation();
	Rotation.Pitch = 0.f;
	GetCapsuleComponent()->SetWorldRotation(FMath::Lerp(AimRotationCurrent, Rotation, coef));
}

void ACharacterBase::AimToNoAimTransitionCallback(float Coef)
{
	SetAimRotation(Coef);
}

ACharacterBase::ETransitionFinished ACharacterBase::FSmoothStateTransitionUpdater::Update(float DeltaTime)
{
	static constexpr auto CubicCurve =
		[](float t)
		{
			float ret = -2*t*t*t + 3*t*t;
			return ret;
		}
	;

	TimeInTransition += DeltaTime;

	Callback(CubicCurve(FMath::Clamp(TimeInTransition / TransitionTime, 0.f, 1.f)));

	if (TimeInTransition >= TransitionTime)
	{
		check(DeltaTime > 0);
		TimeInTransition = TransitionTime;
		return ETransitionFinished::Yes;
	}
	if (TimeInTransition <= 0.f)
	{
		check(DeltaTime < 0);
		TimeInTransition = 0.f;
		return ETransitionFinished::Yes;
	}
	return ETransitionFinished::No;
}

bool ACharacterBase::CanStandUpright() const
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

void ACharacterBase::BeginAttack()
{
	if (CanMakeMeleeAttack())
	{
		SmoothlyOrientSelfToWorldYawValue(GetControlRotation().Yaw);
		MeleeWeaponManagerComponent->BeginAttack();
	}
	else if (CanMakeRangedAttack())
	{
		RangedWeaponManagerComponent->BeginAttack();
	}
}

bool ACharacterBase::CanMakeMeleeAttack() const
{
	return
		AimState == &State_Aim_NoAim
		&& State == &State_UprightNotRunning
		&& !GetCharacterMovement()->IsFalling()
		&& !MeleeWeaponManagerComponent->AttackIsBeingPerformed()
		&& !RangedWeaponManagerComponent->AttackIsBeingPerformed()
	;
}

bool ACharacterBase::CanMakeRangedAttack() const
{
	return
		AimState == &State_Aim_Aim
		&& State != &State_UprightRunning
		&& !MeleeWeaponManagerComponent->AttackIsBeingPerformed()
		&& !RangedWeaponManagerComponent->AttackIsBeingPerformed()
	;
}

void ACharacterBase::EndAttack()
{
	if (MeleeWeaponManagerComponent->AttackIsBeingPerformed())
	{
		MeleeWeaponManagerComponent->EndAttack();
	}
	if (RangedWeaponManagerComponent->AttackIsBeingPerformed())
	{
		RangedWeaponManagerComponent->EndAttack();
	}
}

void ACharacterBase::BeginAim()
{
	Cravings.bWantsToAim = true;
}

void ACharacterBase::EndAim()
{
	Cravings.bWantsToAim = false;
	EndAttack();
}

void ACharacterBase::OnMeleeAttackFinished()
{
	StopSmoothlyOrientSelfToWorldYawValue();
}
void ACharacterBase::OnRangedAttackFinished()
{
}

void ACharacterBase::Landed(const FHitResult& Hit)
{
	auto VelocityZ = -GetVelocity().Z;
	if (VelocityZ < LandingVelocityDamageRange.X)
	{
		return;
	}

	DamageTakerComponent->DamageTaken.Broadcast(NAME_None, FMath::GetMappedRangeValueClamped(LandingVelocityDamageRange, LandingDamageRange, VelocityZ));
}

void ACharacterBase::Die()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeleeWeaponManagerComponent->EndAttack();
	RangedWeaponManagerComponent->EndAttack();

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}

void ACharacterBase::TakeDamageCallback(FName BoneName, float Damage)
{
	if (bIsDead)
	{
		return;
	}
	if (BoneName == HeadBoneName)
	{
		Damage *= HeadShotDamageMultiplier;
	}
	Damage -= Damage * ArmorDamageModifier;
	HealthComponent->Decrease(static_cast<int32>(Damage));
}

void ACharacterBase::TakeMomentumCallback(FName BoneName, FVector ImpactPoint, FVector Momentum)
{
	if (bIsDead)
	{
		if (GetMesh()->IsSimulatingPhysics(BoneName))
		{
			GetMesh()->AddImpulseAtLocation(Momentum, ImpactPoint, BoneName);
		}
	}
	else
	{
		GetCharacterMovement()->AddImpulse(Momentum);
	}
}

void ACharacterBase::WaitForSomeTimeAndStartRegeneratingStaminaIfNeeded(int32 OldStamina, int32 NewStamina)
{
	if (OldStamina <= NewStamina)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(
		StaminaRegenerationTimerHandle
		,
			[this]
			{
				GetWorld()->GetTimerManager().SetTimer(
					StaminaRegenerationTimerHandle
					, 
						[this]
						{
							StaminaComponent->Increase(AmountOfStaminaRegeneration);
							if (StaminaComponent->GetValue() == StaminaComponent->Max)
							{
								GetWorld()->GetTimerManager().ClearTimer(StaminaRegenerationTimerHandle);
							}
						}
					, TimeIntervalForStaminaRegeneration
					, /*bInLoop*/ true
				);
			}
		, TimeBeforeStaminaRegeneration
		, /*bInLoop*/ false
	);
}

void ACharacterBase::RotateSelfIfNeeded(float DeltaTime)
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

void ACharacterBase::SmoothlyOrientSelfToWorldYawValue(float WorldYawValue)
{
	bSmoothlyOrientSelf_Required = true;
	SmoothlyOrientSelf_WorldYawValue = WorldYawValue;
}

void ACharacterBase::StopSmoothlyOrientSelfToWorldYawValue()
{
	bSmoothlyOrientSelf_Required = false;
}

void ACharacterBase::SetMovementSettingsForRunning()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;
}

void ACharacterBase::SetMovementSettingsForNotRunningImpl()
{
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
}

void ACharacterBase::SetMovementSettingsForNotRunning()
{
	if (GetWorld() == nullptr)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(
		MovementSettingsForRunningTimerHandle
		, this
		, &ACharacterBase::SetMovementSettingsForNotRunningImpl
		, DelayForSetMovementSettingsForNotRunning
	);
}

void ACharacterBase::OnWeaponAndAmmoChanged()
{
	if (AimState != &State_Aim_NoAim)
	{
		auto Weapon = RangedWeaponManagerComponent->GetCurrentWeapon();
		check(Weapon != nullptr);
		AnimationSet = Weapon->GetCharacterAnimationSet();
	}
}

FTransform ACharacterBase::GetCurrentWeaponMuzzleSocketTransform() const
{
	auto Transform = GetActorTransform();
	auto Weapon = RangedWeaponManagerComponent->GetCurrentWeapon();
	if (Weapon)
	{
		Transform = Weapon->GetMuzzleSocketTransform();
	}
	return Transform;
}
