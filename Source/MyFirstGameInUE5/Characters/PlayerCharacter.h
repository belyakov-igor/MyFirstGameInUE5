#pragma once

#include "Characters/CharacterBase.h"

#include "CoreMinimal.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* CameraComponent;

	// How much camera should drop down when crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = -200.f, ClampMax = 0.f))
	float SpringArmCrouchSocketOffsetZOffset = -60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	FVector2D SpringArmSocketOffsetXYForAim{60.f, 100.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	float AimingFovCoef = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<class UCameraShakeBase> CameraShake = nullptr;

	virtual void OnWeaponAndAmmoChanged() override;

	virtual bool IsGlobal() const override { return true; }
	virtual FTransform GetDefaultTansform() const override;

	// Properties for game saving
	UPROPERTY(SaveGame)
	TMap<FName /*Level name*/, FName> DesiredPlayerStartNames;

private:

// Damage {
	virtual void Die();
// } Damage

// Action and axis mappings {
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void LookUp(float Amount);
	void LookRight(float Amount);
	void OnMouseWheelInput(float Amount);

	void OnCrouchButtonPressed();
	void OnCrouchButtonReleased();
	void OnRunButtonPressed();
	void OnRunButtonReleased();
	void OnJumpButtonPressed();
// } Action and axis mappings

// Camera pitch {
	float MinCameraPitchBackup = 0.f;
	float MaxCameraPitchBackup = 0.f;

	static constexpr float MinCameraPitch = -80.f;
	static constexpr float MaxCameraPitch = 70.f;
	static constexpr float MinCameraPitchCrouched = -60.f;
	static constexpr float MaxCameraPitchCrouched = 50.f;

	void RestoreBackupCameraPitch();
	void UseUprightCameraPitch(AController* Controller, bool bMakeBackup);
	void UseCrouchCameraPitch(float coef);
// } Camera pitch

// Upright to crouch smooth transition {
	float SpringArmSocketOffsetZBackup = 0.f;

	void SetSpringArmRelativeZ(float coef);

	virtual void UprightToCrouchTransitionCallback(float Coef) override;
// } Upright to crouch smooth transition

// Aim to no aim smooth transition {
	FVector2D SpringArmSocketOffsetXYBackup{0.f, 0.f};
	float FovBackup = 0.f;
	float AimMouseSensitivityCoef = 1.f; // TODO

	// coef == 1 for aim, 0 for no aim
	void SetSpringArmRelativeXY(float coef);
	void SetFov(float coef);

	virtual void AimToNoAimTransitionCallback(float Coef) override;
// } Aim to no aim smooth transition

	UPROPERTY()
	class UWidgetCharacterManHUD* HUDWidget = nullptr;

	void SetMeshOwnerNoSee(bool NoSee);

	UFUNCTION()
	void OnHealthChanged(FName BoneName, float Damage);
};
