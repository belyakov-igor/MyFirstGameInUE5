#include "PlayerCharacter.h"

#include "UI/CharacterManHUDWidget.h"
#include "Interaction/InteractingComponent.h"
#include "Utilities/Components/ClampedIntegerComponent.h"
#include "Utilities/Components/DamageTakerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerCharacter, All, All);

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationYaw = false;
}

void APlayerCharacter::BeginPlay()
{
	SpringArmSocketOffsetZBackup = SpringArmComponent->SocketOffset.Z;
	FovBackup = CameraComponent->FieldOfView;

	SpringArmSocketOffsetXYBackup.X = SpringArmComponent->SocketOffset.X;
	SpringArmSocketOffsetXYBackup.Y = SpringArmComponent->SocketOffset.Y;
	
	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateWeaponAndAmmo();
		HUDWidget->UpdateHealth();
		HUDWidget->UpdateStamina();
		HUDWidget->UpdateCrosshairVisibility(false);
		if (auto InteractingComponent_ = FindComponentByClass<UInteractingComponent>(); InteractingComponent != nullptr)
		{
			InteractingComponent_->ChangeHUDText.AddUObject(HUDWidget, &UCharacterManHUDWidget::ChangeInteractingHUDText);
		}
	}

	DamageTakerComponent->DamageTaken.AddDynamic(this, &APlayerCharacter::OnHealthChanged);

	Super::BeginPlay();
}

void APlayerCharacter::PossessedBy(AController* NewController)
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

void APlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	RestoreBackupCameraPitch();
	if (HUDWidget != nullptr)
	{
		HUDWidget->RemoveFromParent();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateCrosshairVisibility(IsInAimingState());
	}

	FVector ClosestPoint;
	SetMeshOwnerNoSee(GetCapsuleComponent()->GetDistanceToCollision(CameraComponent->GetComponentLocation(), ClosestPoint) == 0.f);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent != nullptr);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);
	PlayerInputComponent->BindAxis("MouseWheel", this, &APlayerCharacter::OnMouseWheelInput);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnCrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::OnCrouchButtonReleased);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnRunButtonPressed);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacter::OnRunButtonReleased);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnJumpButtonPressed);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &ACharacterBase::BeginAttack);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Released, this, &ACharacterBase::EndAttack);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ACharacterBase::BeginAim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ACharacterBase::EndAim);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, RangedWeaponManagerComponent, &UWeaponManagerComponent::Reload);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, InteractingComponent, &UInteractingComponent::Interact);

	PlayerInputComponent->BindAction("Weapon 1", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon1Pressed);
	PlayerInputComponent->BindAction("Weapon 2", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon2Pressed);
	PlayerInputComponent->BindAction("Weapon 3", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon3Pressed);
	PlayerInputComponent->BindAction("Weapon 4", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon4Pressed);
	PlayerInputComponent->BindAction("Weapon 5", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon5Pressed);
	PlayerInputComponent->BindAction("Weapon 6", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon6Pressed);
	PlayerInputComponent->BindAction("Weapon 7", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon7Pressed);
	PlayerInputComponent->BindAction("Weapon 8", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon8Pressed);
	PlayerInputComponent->BindAction("Weapon 9", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon9Pressed);
	PlayerInputComponent->BindAction("Weapon 0", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnWeapon0Pressed);
}

void APlayerCharacter::MoveForward(float Amount)
{
	auto dir = CameraComponent->GetForwardVector();
	dir.Z = 0;
	dir.Normalize();
	AddMovementInput(dir, Amount);
}

void APlayerCharacter::MoveRight(float Amount)
{
	AddMovementInput(CameraComponent->GetRightVector(), Amount);
}

void APlayerCharacter::LookUp(float Amount)
{
	AddControllerPitchInput(Amount*AimMouseSensitivityCoef);
}

void APlayerCharacter::LookRight(float Amount)
{
	AddControllerYawInput(Amount*AimMouseSensitivityCoef);
}

void APlayerCharacter::OnMouseWheelInput(float Amount)
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

void APlayerCharacter::OnCrouchButtonPressed()
{
	Cravings.bWantsToCrouch = true;
}

void APlayerCharacter::OnCrouchButtonReleased()
{
	Cravings.bWantsToCrouch = false;
}

void APlayerCharacter::OnRunButtonPressed()
{
	Cravings.bWantsToRun = true;
}

void APlayerCharacter::OnRunButtonReleased()
{
	Cravings.bWantsToRun = false;
}

void APlayerCharacter::OnJumpButtonPressed()
{
	if (!IsInCrouchState()) // TODO
	{
		Jump();
	}
}

void APlayerCharacter::OnWeapon1Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(1); }
void APlayerCharacter::OnWeapon2Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(2); }
void APlayerCharacter::OnWeapon3Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(3); }
void APlayerCharacter::OnWeapon4Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(4); }
void APlayerCharacter::OnWeapon5Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(5); }
void APlayerCharacter::OnWeapon6Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(6); }
void APlayerCharacter::OnWeapon7Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(7); }
void APlayerCharacter::OnWeapon8Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(8); }
void APlayerCharacter::OnWeapon9Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(9); }
void APlayerCharacter::OnWeapon0Pressed() { RangedWeaponManagerComponent->SetCurrentWeaponSlot(0); }

void APlayerCharacter::RestoreBackupCameraPitch()
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

void APlayerCharacter::UseUprightCameraPitch(AController* Controller_, bool bMakeBackup)
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

void APlayerCharacter::UseCrouchCameraPitch(float coef)
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

void APlayerCharacter::SetSpringArmRelativeZ(float coef)
{
	SpringArmComponent->SocketOffset.Z = SpringArmSocketOffsetZBackup + FMath::Lerp(0, SpringArmCrouchSocketOffsetZOffset, coef);
}

void APlayerCharacter::UprightToCrouchTransitionCallback(float Coef)
{
	Super::UprightToCrouchTransitionCallback(Coef);
	SetSpringArmRelativeZ(Coef);
	UseCrouchCameraPitch(Coef);
}

void APlayerCharacter::SetSpringArmRelativeXY(float coef)
{
	FVector2D Offset = FMath::Lerp(SpringArmSocketOffsetXYBackup, SpringArmSocketOffsetXYForAim, coef);
	SpringArmComponent->SocketOffset.X = Offset.X;
	SpringArmComponent->SocketOffset.Y = Offset.Y;
}

void APlayerCharacter::SetFov(float coef)
{
	CameraComponent->FieldOfView = FMath::Lerp(FovBackup, FovBackup*AimingFovCoef, coef);
	AimMouseSensitivityCoef = 1 - coef*AimingFovCoef*0.5f;
}

void APlayerCharacter::AimToNoAimTransitionCallback(float Coef)
{
	Super::AimToNoAimTransitionCallback(Coef);
	SetSpringArmRelativeXY(Coef);
	SetFov(Coef);
}

void APlayerCharacter::OnWeaponAndAmmoChanged()
{
	Super::OnWeaponAndAmmoChanged();
	HUDWidget->UpdateWeaponAndAmmo();
}

void APlayerCharacter::Die()
{
	Super::Die();

	if (HUDWidget != nullptr)
	{
		HUDWidget->RemoveFromParent();
	}
}

void APlayerCharacter::SetMeshOwnerNoSee(bool NoSee)
{
	GetMesh()->SetOwnerNoSee(NoSee);

	TArray<USceneComponent*> SceneChildren;
	GetMesh()->GetChildrenComponents(true, SceneChildren);

	for (auto SceneChild : SceneChildren)
	{
		auto PrimitiveChild = Cast<UPrimitiveComponent>(SceneChild);
		if (PrimitiveChild != nullptr)
		{
			PrimitiveChild->SetOwnerNoSee(NoSee);
		}
	}
}

void APlayerCharacter::OnHealthChanged(FName BoneName, float Damage)
{
	if (HUDWidget != nullptr)
	{
		HUDWidget->HealthDecreased();
	}

	auto PlayerController = GetController<APlayerController>();
	if (PlayerController == nullptr || PlayerController->PlayerCameraManager == nullptr)
	{
		return;
	}
	PlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
}
