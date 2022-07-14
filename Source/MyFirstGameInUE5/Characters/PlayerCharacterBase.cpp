#include "PlayerCharacterBase.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PlayerController = Cast<APlayerController>(NewController); PlayerController != nullptr)
	{
		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager; PlayerCameraManager != nullptr)
		{
			MinCameraPitchBackup = PlayerCameraManager->ViewPitchMin;
			MaxCameraPitchBackup = PlayerCameraManager->ViewPitchMax;

			PlayerCameraManager->ViewPitchMin = MinCameraPitch;
			PlayerCameraManager->ViewPitchMax = MaxCameraPitch;
		}
	}
}

void APlayerCharacterBase::UnPossessed()
{
	Super::UnPossessed();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController != nullptr)
	{
		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager; PlayerCameraManager != nullptr)
		{
			PlayerCameraManager->ViewPitchMin = MinCameraPitchBackup;
			PlayerCameraManager->ViewPitchMax = MaxCameraPitchBackup;
		}
	}
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Velocity = GetVelocity();
	Acceleration = (Velocity - PreviousVelocity) / DeltaTime;
	PreviousVelocity = Velocity;
}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacterBase::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacterBase::LookRight);

	PlayerInputComponent->BindAction("Croach", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnCroachButtonPressed);
	PlayerInputComponent->BindAction("Croach", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnCroachButtonReleased);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacterBase::OnRunButtonPressed);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacterBase::OnRunButtonReleased);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
}

bool APlayerCharacterBase::IsRunning() const
{
	return bWantsToRun && GetCharacterMovement()->Velocity.SquaredLength() > 2500;
}

bool APlayerCharacterBase::IsCroaching() const
{
	return bWantsToCroach;
}

bool APlayerCharacterBase::HasAnyMovementInput() const
{
	return HasMoveForwardInput || HasMoveRightInput;
}

FVector APlayerCharacterBase::GetAcceleration() const
{
	return Acceleration;
}

float APlayerCharacterBase::GetBackwardAcceleration() const
{
	return -FVector::DotProduct(Acceleration, GetCapsuleComponent()->GetForwardVector());
}

void APlayerCharacterBase::MoveForward(float Amount)
{
	auto dir = CameraComponent->GetForwardVector();
	dir.Z = 0;
	dir.Normalize();
	AddMovementInput(dir, Amount);
	HasMoveForwardInput = Amount != 0.f;
}

void APlayerCharacterBase::MoveRight(float Amount)
{
	AddMovementInput(CameraComponent->GetRightVector(), Amount);
	HasMoveRightInput = Amount != 0.f;
}

void APlayerCharacterBase::LookUp(float Amount)
{
	AddControllerPitchInput(Amount);
}

void APlayerCharacterBase::LookRight(float Amount)
{
	AddControllerYawInput(Amount);
}

void APlayerCharacterBase::OnCroachButtonPressed()
{
	if (!bWantsToRun) bWantsToCroach = true;
}

void APlayerCharacterBase::OnCroachButtonReleased()
{
	bWantsToCroach = false;
}

void APlayerCharacterBase::OnRunButtonPressed()
{
	if (!bWantsToCroach)
	{
		bWantsToRun = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeedCoef * JogSpeed;
	}
}

void APlayerCharacterBase::OnRunButtonReleased()
{
	bWantsToRun = false;
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
}
