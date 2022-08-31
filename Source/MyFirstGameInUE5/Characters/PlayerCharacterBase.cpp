#include "PlayerCharacterBase.h"

#include "Utilities/Components/ClampedIntegerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Actors/BaseWeapon.h"
#include "UI/CharacterManHUDWidget.h"
#include "Interaction/InteractingComponent.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"

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

	HealthComponent = CreateDefaultSubobject<UClampedIntegerComponent>(HealthComponentName);
	StaminaComponent = CreateDefaultSubobject<UClampedIntegerComponent>(StaminaComponentName);
	MeleeWeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>(MeleeWeaponManagerComponentName);
	MeleeWeaponManagerComponent->OnAttackFinished.BindUObject(this, &APlayerCharacterBase::OnMeleeAttackFinished);
	RangedWeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>(RangedWeaponManagerComponentName);
	RangedWeaponManagerComponent->OnAttackFinished.BindUObject(this, &APlayerCharacterBase::OnRangedAttackFinished);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractingComponent = CreateDefaultSubobject<UInteractingComponent>("InteractingComponent");
	InteractingComponent->SetupAttachment(CameraComponent);

	DamageTakerComponent = CreateDefaultSubobject<UDamageTakerComponent>("DamageTakerComponent");

	check(GetCharacterMovement() != nullptr);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
	SetMovementSettingsForNotRunning();

	SetState(State_UprightNotRunning);
	SetAimState(State_Aim_NoAim);
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	check(State != nullptr);
	check(AimState != nullptr);

	HealthComponent->ReachedMin.AddUObject(this, &APlayerCharacterBase::Die);
	HealthComponent->SetValue(HealthComponent->Max);

	StaminaComponent->SetValue(StaminaComponent->Max);
	StaminaComponent->ValueChanged.AddUObject(this, &APlayerCharacterBase::WaitForSomeTimeAndStartRegeneratingStaminaIfNeeded);

	RangedWeaponManagerComponent->OnWeaponAndAmmoChanged.BindUObject(this, &APlayerCharacterBase::OnWeaponAndAmmoChanged);

	{
		checkf(DefaultMeleeWeaponClass != nullptr, TEXT("Default melee weapon should be specified."));
		auto DefaultMeleeWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultMeleeWeaponClass);
		MeleeWeaponManagerComponent->AddWeapon(DefaultMeleeWeapon);
	}


	CapsuleUprightHalfHeightBackup = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	SpringArmSocketOffsetZBackup = SpringArmComponent->SocketOffset.Z;
	MeshZOffsetFromCapsuleLowestPointBackup = GetMesh()->GetRelativeLocation().Z + GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	FovBackup = CameraComponent->FieldOfView;

	UprightToCrouchUpdater.TransitionTime = UprightToCrouchTransitionTime;
	UprightToCrouchUpdater.Callback =
		[this](float Coef)
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
			SetSpringArmRelativeZ(Coef);
			SetVelocityAccordingToCrouch(Coef);
			UseCrouchCameraPitch(Coef);
		}
	;
	UprightToCrouchUpdater.Callback(0.f);


	SpringArmSocketOffsetXYBackup.X = SpringArmComponent->SocketOffset.X;
	SpringArmSocketOffsetXYBackup.Y = SpringArmComponent->SocketOffset.Y;
	
	AimToNoAimUpdater.TransitionTime = AimToNoAimTransitionTime;
	AimToNoAimUpdater.Callback =
		[this](float Coef)
		{
			SetSpringArmRelativeXY(Coef);
			SetAimRotation(Coef);
			SetFov(Coef);
		}
	;
	AimToNoAimUpdater.Callback(0.f);

	DamageTakerComponent->DamageTaken.AddDynamic(this, &APlayerCharacterBase::TakeDamageCallback);
	DamageTakerComponent->MomentumTaken.AddDynamic(this, &APlayerCharacterBase::TakeMomentumCallback);

	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateWeaponAndAmmo();
		HUDWidget->UpdateHealth();
		HUDWidget->UpdateStamina();
		HUDWidget->UpdateCrosshairVisibility(false);
		InteractingComponent->ChangeHUDText.AddUObject(HUDWidget, &UCharacterManHUDWidget::ChangeInteractingHUDText);
	}
}

void APlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController != nullptr && Cast<APlayerController>(NewController) != nullptr)
	{
		UseUprightCameraPitch(NewController, true);
		HUDWidget = CreateWidget<UCharacterManHUDWidget>(GetWorld(), HUDWidgetClass, "HUD");
		check(HUDWidget != nullptr);
		HUDWidget->AddToViewport();
		HealthComponent->ValueChanged.AddLambda([this](int32, int32){ HUDWidget->UpdateHealth(); });
		StaminaComponent->ValueChanged.AddLambda([this](int32, int32) { HUDWidget->UpdateStamina(); });
	}
}

void APlayerCharacterBase::UnPossessed()
{
	Super::UnPossessed();

	RestoreBackupCameraPitch();
	if (HUDWidget != nullptr)
	{
		HUDWidget->RemoveFromParent();
	}
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	State->Tick(DeltaTime);
	AimState->Tick(DeltaTime);

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
	PlayerInputComponent->BindAxis("MouseWheel", this, &APlayerCharacterBase::OnMouseWheelInput);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnCrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnCrouchButtonReleased);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnRunButtonPressed);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnRunButtonReleased);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnJumpButtonPressed);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::BeginAttack);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Released, this, &APlayerCharacterBase::EndAttack);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::BeginAim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &APlayerCharacterBase::EndAim);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, RangedWeaponManagerComponent, &UWeaponManagerComponent::Reload);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, InteractingComponent, &UInteractingComponent::Interact);

	PlayerInputComponent->BindAction("Weapon 1", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon1Pressed);
	PlayerInputComponent->BindAction("Weapon 2", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon2Pressed);
	PlayerInputComponent->BindAction("Weapon 3", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon3Pressed);
	PlayerInputComponent->BindAction("Weapon 4", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon4Pressed);
	PlayerInputComponent->BindAction("Weapon 5", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon5Pressed);
	PlayerInputComponent->BindAction("Weapon 6", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon6Pressed);
	PlayerInputComponent->BindAction("Weapon 7", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon7Pressed);
	PlayerInputComponent->BindAction("Weapon 8", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon8Pressed);
	PlayerInputComponent->BindAction("Weapon 9", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon9Pressed);
	PlayerInputComponent->BindAction("Weapon 0", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnWeapon0Pressed);
}

bool APlayerCharacterBase::IsInRunState() const
{
	return State == &State_UprightRunning;
}

bool APlayerCharacterBase::IsInCrouchState() const
{
	return State == &State_Crouch;
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

bool APlayerCharacterBase::AttackIsBeingPerformed() const
{
	return MeleeWeaponManagerComponent->AttackIsBeingPerformed() || RangedWeaponManagerComponent->AttackIsBeingPerformed();
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
	AddControllerPitchInput(Amount*AimMouseSensitivityCoef);
}

void APlayerCharacterBase::LookRight(float Amount)
{
	AddControllerYawInput(Amount*AimMouseSensitivityCoef);
}

void APlayerCharacterBase::OnMouseWheelInput(float Amount)
{
	if (Amount == 0)
	{
		return;
	}
	if (Amount > 0)
	{
		RangedWeaponManagerComponent->SetNextWeapon();
	}
	else
	{
		RangedWeaponManagerComponent->SetPreviousWeapon();
	}
}

void APlayerCharacterBase::OnCrouchButtonPressed()
{
	Cravings.bWantsToCrouch = true;
}

void APlayerCharacterBase::OnCrouchButtonReleased()
{
	Cravings.bWantsToCrouch = false;
}

void APlayerCharacterBase::OnRunButtonPressed()
{
	Cravings.bWantsToRun = true;
}

void APlayerCharacterBase::OnRunButtonReleased()
{
	Cravings.bWantsToRun = false;
}

void APlayerCharacterBase::OnJumpButtonPressed()
{
	if (State != &State_Crouch)
	{
		Jump();
	}
}

void APlayerCharacterBase::OnWeapon1Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(1); }
void APlayerCharacterBase::OnWeapon2Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(2); }
void APlayerCharacterBase::OnWeapon3Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(3); }
void APlayerCharacterBase::OnWeapon4Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(4); }
void APlayerCharacterBase::OnWeapon5Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(5); }
void APlayerCharacterBase::OnWeapon6Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(6); }
void APlayerCharacterBase::OnWeapon7Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(7); }
void APlayerCharacterBase::OnWeapon8Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(8); }
void APlayerCharacterBase::OnWeapon9Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(9); }
void APlayerCharacterBase::OnWeapon0Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(0); }

bool APlayerCharacterBase::RunningIsPossible() const
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

void APlayerCharacterBase::FState_UprightNotRunning::Tick(float DeltaTime)
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

void APlayerCharacterBase::FState_UprightNotRunning::TakeOver()
{
	Character.GetCharacterMovement()->MaxWalkSpeed = Character.DefaultJogSpeed;
}

// } FState_UprightNotRunning ---------------------------------------------------------------------------


// FState_UprightRunning { ------------------------------------------------------------------------------

void APlayerCharacterBase::FState_UprightRunning::Tick(float DeltaTime)
{
	if (!Character.RunningIsPossible())
	{
		Character.GetWorld()->GetTimerManager().ClearTimer(Character.StaminaDecreasingTimerHandle);
		Character.SetMovementSettingsForNotRunning();
		Character.SetState(Character.State_UprightNotRunning);
	}
}

void APlayerCharacterBase::FState_UprightRunning::TakeOver()
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

void APlayerCharacterBase::FState_Crouch::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToCrouch && Character.CanStandUpright())
	{
		Character.SetState(Character.State_TransitionCrouchToUprightNotRunning);
	}
}

void APlayerCharacterBase::FState_Crouch::TakeOver()
{
}

// } FState_Crouch --------------------------------------------------------------------------------------


// FState_TransitionUprightNotRunningToCrouch { ---------------------------------------------------------

void APlayerCharacterBase::FState_TransitionUprightNotRunningToCrouch::Tick(float DeltaTime)
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

void APlayerCharacterBase::FState_TransitionUprightNotRunningToCrouch::TakeOver()
{
}

// } FState_TransitionUprightNotRunningToCrouch ---------------------------------------------------------


// FState_TransitionCrouchToUprightNotRunning { ---------------------------------------------------------

void APlayerCharacterBase::FState_TransitionCrouchToUprightNotRunning::Tick(float DeltaTime)
{
	if (Character.Cravings.bWantsToCrouch)
	{
		Character.SetState(Character.State_TransitionUprightNotRunningToCrouch);
		return;
	}
	if (Character.UprightToCrouchUpdater.Update(-DeltaTime) == ETransitionFinished::Yes)
	{
		Character.CrouchCoef = 0.f;
		Character.UseUprightCameraPitch(Character.Controller, false);
		Character.SetState(Character.State_UprightNotRunning);
	}
}

void APlayerCharacterBase::FState_TransitionCrouchToUprightNotRunning::TakeOver()
{
	Character.SetCapsuleHalfHeight(0.f);
}

// } FState_TransitionCrouchToUprightNotRunning ---------------------------------------------------------




// FState_Aim_NoAim { -----------------------------------------------------------------------------------

void APlayerCharacterBase::FState_Aim_NoAim::Tick(float DeltaTime)
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

void APlayerCharacterBase::FState_Aim_NoAim::TakeOver()
{
	Character.GetCharacterMovement()->bOrientRotationToMovement = true;
	Character.GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

// } FState_Aim_NoAim -----------------------------------------------------------------------------------


// FState_Aim_Aim { -------------------------------------------------------------------------------------

void APlayerCharacterBase::FState_Aim_Aim::Tick(float DeltaTime)
{
	if (!Character.Cravings.bWantsToAim)
	{
		Character.SetAimState(Character.State_Aim_TransitionAimToNoAim);
	}
}

void APlayerCharacterBase::FState_Aim_Aim::TakeOver()
{
	Character.GetCharacterMovement()->bOrientRotationToMovement = false;
	Character.GetCharacterMovement()->bUseControllerDesiredRotation = true;
	Character.HUDWidget->UpdateCrosshairVisibility(true);
}

// } FState_Aim_Aim -------------------------------------------------------------------------------------


// FState_Aim_TransitionNoAimToAim { --------------------------------------------------------------------

void APlayerCharacterBase::FState_Aim_TransitionNoAimToAim::Tick(float DeltaTime)
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

void APlayerCharacterBase::FState_Aim_TransitionNoAimToAim::TakeOver()
{
	Character.AimRotationCurrent = Character.GetCapsuleComponent()->GetComponentRotation();
	Character.RangedWeaponManagerComponent->SetWeaponVisibility(true);
	auto Weapon = Character.RangedWeaponManagerComponent->GetCurrentWeapon();
	check(Weapon != nullptr);
	Character.AnimationSet = Weapon->GetCharacterAnimationSet();
}

// } FState_Aim_TransitionNoAimToAim --------------------------------------------------------------------


// FState_Aim_TransitionAimToNoAim { --------------------------------------------------------------------

void APlayerCharacterBase::FState_Aim_TransitionAimToNoAim::Tick(float DeltaTime)
{
	if (Character.Cravings.bWantsToAim)
	{
		Character.SetAimState(Character.State_Aim_TransitionNoAimToAim);
		return;
	}
	if (Character.AimToNoAimUpdater.Update(-DeltaTime) == ETransitionFinished::Yes)
	{
		Character.RangedWeaponManagerComponent->SetWeaponVisibility(false);
		Character.AnimationSet = EPlayerCharacterBaseAnimationSet::Unarmed;
		Character.SetAimState(Character.State_Aim_NoAim);
	}
}

void APlayerCharacterBase::FState_Aim_TransitionAimToNoAim::TakeOver()
{
	auto Rotation = Character.CameraComponent->GetComponentRotation();
	Rotation.Pitch = 0.f;
	Character.AimRotationCurrent = Rotation;
	Character.RangedWeaponManagerComponent->EndAttack();
	Character.HUDWidget->UpdateCrosshairVisibility(false);
}

// } FState_Aim_TransitionAimToNoAim --------------------------------------------------------------------


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
	float HalfHeightOld = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeight = FMath::Lerp(CapsuleUprightHalfHeightBackup, CapsuleCrouchHalfHeight, coef);
	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeight);
	auto location = GetCapsuleComponent()->GetComponentLocation();
	location.Z -= HalfHeightOld - HalfHeight;
	GetCapsuleComponent()->SetWorldLocation(location);

	location = GetMesh()->GetRelativeLocation();
	location.Z = -HalfHeight + MeshZOffsetFromCapsuleLowestPointBackup;
	GetMesh()->SetRelativeLocation(location);

	location = SpringArmComponent->GetRelativeLocation();
	location.Z = CapsuleUprightHalfHeightBackup - HalfHeight;
	SpringArmComponent->SetRelativeLocation(location);
}

void APlayerCharacterBase::SetVelocityAccordingToCrouch(float coef)
{
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(1, CrouchVelocityCoef, coef) * DefaultJogSpeed;
}

void APlayerCharacterBase::SetSpringArmRelativeXY(float coef)
{
	FVector2D Offset = FMath::Lerp(SpringArmSocketOffsetXYBackup, SpringArmSocketOffsetXYForAim, coef);
	SpringArmComponent->SocketOffset.X = Offset.X;
	SpringArmComponent->SocketOffset.Y = Offset.Y;
}

void APlayerCharacterBase::SetAimRotation(float coef)
{
	FRotator Rotation = CameraComponent->GetComponentRotation();
	Rotation.Pitch = 0.f;
	GetCapsuleComponent()->SetWorldRotation(FMath::Lerp(AimRotationCurrent, Rotation, coef));
}

void APlayerCharacterBase::SetFov(float coef)
{
	CameraComponent->FieldOfView = FMath::Lerp(FovBackup, FovBackup*AimingFovCoef, coef);
	AimMouseSensitivityCoef = 1 - coef*AimingFovCoef*0.5f;
}

APlayerCharacterBase::ETransitionFinished APlayerCharacterBase::FSmoothStateTransitionUpdater::Update(float DeltaTime)
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

void APlayerCharacterBase::BeginAttack()
{
	if (CanMakeMeleeAttack())
	{
		SmoothlyOrientSelfToWorldYawValue(CameraComponent->GetComponentRotation().Yaw);
		MeleeWeaponManagerComponent->BeginAttack();
		return;
	}
	if (CanMakeRangedAttack())
	{
		RangedWeaponManagerComponent->BeginAttack();
	}
}

bool APlayerCharacterBase::CanMakeMeleeAttack() const
{
	return
		AimState == &State_Aim_NoAim
		&& State == &State_UprightNotRunning
		&& !GetCharacterMovement()->IsFalling()
		&& !MeleeWeaponManagerComponent->AttackIsBeingPerformed()
		&& !RangedWeaponManagerComponent->AttackIsBeingPerformed()
	;
}

bool APlayerCharacterBase::CanMakeRangedAttack() const
{
	return
		AimState == &State_Aim_Aim
		&& State != &State_UprightRunning
		&& !MeleeWeaponManagerComponent->AttackIsBeingPerformed()
		&& !RangedWeaponManagerComponent->AttackIsBeingPerformed()
	;
}

void APlayerCharacterBase::EndAttack()
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

void APlayerCharacterBase::BeginAim()
{
	Cravings.bWantsToAim = true;
}

void APlayerCharacterBase::EndAim()
{
	Cravings.bWantsToAim = false;
	EndAttack();
}

void APlayerCharacterBase::OnMeleeAttackFinished()
{
	StopSmoothlyOrientSelfToWorldYawValue();
}
void APlayerCharacterBase::OnRangedAttackFinished()
{
}

void APlayerCharacterBase::Landed(const FHitResult& Hit)
{
	auto VelocityZ = -GetVelocity().Z;
	if (VelocityZ < LandingVelocityDamageRange.X)
	{
		return;
	}

	TakeDamageCallback(NAME_None, FMath::GetMappedRangeValueClamped(LandingVelocityDamageRange, LandingDamageRange, VelocityZ));
}

void APlayerCharacterBase::OnWeaponAndAmmoChanged()
{
	HUDWidget->UpdateWeaponAndAmmo();

	if (AimState != &State_Aim_NoAim)
	{
		auto Weapon = RangedWeaponManagerComponent->GetCurrentWeapon();
		check(Weapon != nullptr);
		AnimationSet = Weapon->GetCharacterAnimationSet();
	}
}

void APlayerCharacterBase::Die()
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
	if (IsPlayerControlled())
	{
		HUDWidget->RemoveFromParent();
	}

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}

void APlayerCharacterBase::TakeDamageCallback(FName BoneName, float Damage)
{
	if (bIsDead)
	{
		return;
	}
	if (BoneName == HeadBoneName)
	{
		Damage *= HeadShotDamageMultiplier;
	}
	HealthComponent->Decrease(static_cast<int32>(Damage));
}

void APlayerCharacterBase::TakeMomentumCallback(FName BoneName, FVector ImpactPoint, FVector Momentum)
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

void APlayerCharacterBase::WaitForSomeTimeAndStartRegeneratingStaminaIfNeeded(int32 OldStamina, int32 NewStamina)
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

void APlayerCharacterBase::SetMovementSettingsForRunning()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;
}

void APlayerCharacterBase::SetMovementSettingsForNotRunningImpl()
{
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
}

void APlayerCharacterBase::SetMovementSettingsForNotRunning()
{
	if (GetWorld() == nullptr)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(
		MovementSettingsForRunningTimerHandle
		, this
		, &APlayerCharacterBase::SetMovementSettingsForNotRunningImpl
		, DelayForSetMovementSettingsForNotRunning
	);
}
