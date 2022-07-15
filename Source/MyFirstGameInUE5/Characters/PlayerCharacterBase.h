// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "PlayerCharacterBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 1.2f, ClampMax = 4.f))
	float RunSpeedCoef = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float CrouchVelocityCoef = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 100.f, ClampMax = 1000.f))
	float DefaultJogSpeed = 500.f;

	// How much camera should drop down when crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = -200.f, ClampMax = 0.f))
	float SpringArmCrouchSocketOffsetZOffset = -60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 10.f, ClampMax = 100.f))
	float CapsuleCrouchHalfHeight = 58.f;

	// How much time takes transitin from upright to crouch and vice-versa
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 0.f, ClampMax = 3.f))
	float TransitionTime = 0.5f;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsInCrouchState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetForwardMovementInput() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetRightMovementInput() const;

	// result in range [0, 1]. 0 is upright, 1 is crouch
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetCrouchCoef() const;

private:

// Action and axis mappings {
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void LookUp(float Amount);
	void LookRight(float Amount);

	void OnCrouchButtonPressed();
	void OnCrouchButtonReleased();
	void OnRunButtonPressed();
	void OnRunButtonReleased();
	void OnJumpButtonPressed();
// } Action and axis mappings

// Locomotion helpers {
	bool bWantsToCrouch = false;
	bool bIsCrouching = false;
	void CrouchIfPossible();

	bool bWantsToRun = false;
	bool bIsRunning = false;
	void RunIfPossible();
// } Locomotion helpers

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

// Upright to crouch smooth camera and capsule transition {
	float CapsuleUprightHalfHeightBackup = 0.f;
	float SpringArmSocketOffsetZBackup = 0.f;
	float MeshZOffsetFromCapsuleLowestPointBackup = 0.f;

	// coef == 1 for upright, 0 for crouching
	void SetSpringArmRelativeZ(float coef);
	void SetCapsuleHalfHeight(float coef);
	void SetVelocityAccordingToCrouch(float coef);

	enum class ECrouchSequenceType : int
	{
		UPRIGHT_TO_CROUCH = 1
		, CROUCH_TO_UPRIGHT = -1
		, NONE = 0
	};
	ECrouchSequenceType CrouchSequenceType = ECrouchSequenceType::NONE;
	float TimeInTransition = 0.f;

	void UpdateUprightToCrouchSmoothCameraAndCapsuleTransition(float DeltaTime);
// } Upright to crouch smooth camera and capsule transition

// Upright to crouch switch {
	float CrouchCoef = 0.f;
	bool bWantsToStandUpright = false;

	void ToCrouchState();
	void ToUprightState();
	bool CanStandUpright() const;
	void StandUprightIfPossible();
// } Upright to crouch switch

};
