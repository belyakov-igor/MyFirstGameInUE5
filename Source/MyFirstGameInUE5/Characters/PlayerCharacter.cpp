#include "PlayerCharacter.h"

#include "UI/WidgetCharacterManHUD.h"
#include "Interaction/InteractingComponent.h"
#include "Utilities/Components/ClampedIntegerComponent.h"
#include "Utilities/Components/DamageTakerComponent.h"
#include "Weapons/Components/WeaponManagerComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerCharacter, All, All);

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractingComponent->SetupAttachment(CameraComponent);

	bUseControllerRotationYaw = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void APlayerCharacter::BeginPlay()
{
	SpringArmSocketOffsetZBackup = SpringArmComponent->SocketOffset.Z;
	FovBackup = CameraComponent->FieldOfView;

	SpringArmSocketOffsetXYBackup.X = SpringArmComponent->SocketOffset.X;
	SpringArmSocketOffsetXYBackup.Y = SpringArmComponent->SocketOffset.Y;

	DamageTakerComponent->DamageTaken.AddDynamic(this, &APlayerCharacter::OnHealthChanged);

	Super::BeginPlay();

	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateWeaponAndAmmo();
		HUDWidget->UpdateHealth();
		HUDWidget->UpdateStamina();
		HUDWidget->UpdateCrosshairVisibility(false);
		if (auto InteractingComponent_ = FindComponentByClass<UInteractingComponent>(); InteractingComponent != nullptr)
		{
			InteractingComponent_->ChangeHUDText.AddUObject(HUDWidget, &UWidgetCharacterManHUD::ChangeInteractingHUDText);
		}
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController != nullptr && Cast<APlayerController>(NewController) != nullptr)
	{
		UseUprightCameraPitch(NewController, true);
		HUDWidget = CreateWidget<UWidgetCharacterManHUD>(GetWorld(), HUDWidgetClass, "HUD");
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

	auto WM_SetCurrentWeaponSlot = &UWeaponManagerComponent::SetCurrentWeaponSlot;
	auto Pressed = EInputEvent::IE_Pressed;
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 1", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 1);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 2", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 2);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 3", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 3);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 4", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 4);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 5", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 5);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 6", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 6);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 7", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 7);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 8", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 8);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 9", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 9);
	PlayerInputComponent->BindAction<FInt32Signature>("Weapon 0", Pressed, RangedWeaponManagerComponent, WM_SetCurrentWeaponSlot, 0);
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

FTransform APlayerCharacter::GetDefaultTansform() const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	auto Default = [&Actors]{ return Actors.IsEmpty() ? FTransform{} : Actors[0]->GetActorTransform(); };

	auto DesiredPlayerStartName = DesiredPlayerStartNames.Find(FName(UGameplayStatics::GetCurrentLevelName(GetWorld())));
	if (DesiredPlayerStartName == nullptr)
	{
		return Default();
	}

	AActor** ActorPtr = Actors.FindByPredicate([DesiredPlayerStartName](const AActor* Actor){ return Actor->GetFName() == *DesiredPlayerStartName; });
	if (ActorPtr == nullptr)
	{
		return Default();
	}
	else
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(*ActorPtr);
		if (PlayerStart == nullptr)
		{
			check(false);
			return Default();
		}
		else
		{
			return PlayerStart->GetActorTransform();
		}
	}
}
