#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "PlayerCharacterBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UClampedIntegerComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UClampedIntegerComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UWeaponManagerComponent* MeleeWeaponManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UWeaponManagerComponent* RangedWeaponManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UInteractingComponent* InteractingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UDamageTakerComponent* DamageTakerComponent;


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
	float UprightToCrouchTransitionTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 0.f))
	float DelayForSetMovementSettingsForNotRunning = 1.0f;

	// How much time takes transitin from no aim to aim and vice-versa
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = 0.f, ClampMax = 3.f))
	float AimToNoAimTransitionTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	FVector2D LandingVelocityDamageRange{900.f, 1500.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float HeadShotDamageMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	FName HeadBoneName = "head";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	FVector2D LandingDamageRange{10.f, 100.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	FVector2D SpringArmSocketOffsetXYForAim{60.f, 100.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	float AimingFovCoef = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class ABaseWeapon> DefaultMeleeWeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	TEnumAsByte<EPlayerCharacterBaseAnimationSet> AnimationSet = EPlayerCharacterBaseAnimationSet::Unarmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0.001f))
	float TimeIntervalForStaminaDecresingOnRunning = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0))
	int32 AmountOfStaminaDecresingOnRunning = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0.f))
	float TimeBeforeStaminaRegeneration = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0.01f))
	float TimeIntervalForStaminaRegeneration = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0))
	int32 AmountOfStaminaRegeneration = 1;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsInRunState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsInCrouchState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attack")
	bool IsInNoAimingState() const { return AimState == &State_Aim_NoAim; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attack")
	bool IsInAimingState() const { return AimState == &State_Aim_Aim; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetForwardMovementInput() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetRightMovementInput() const;

	// result in range [0, 1]. 0 is upright, 1 is crouch
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetCrouchCoef() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attack")
	bool AttackIsBeingPerformed() const;



// SmoothlyOrientSelfToWorldYawValue facilities {
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion")
	float InterpSpeedToSmoothlyOrientSelf = 5.f;

	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	void SmoothlyOrientSelfToWorldYawValue(float WorldYawValue);

	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	void StopSmoothlyOrientSelfToWorldYawValue();
// } SmoothlyOrientSelfToWorldYawValue facilities


	virtual void Landed(const FHitResult& Hit) override;

	void OnWeaponAndAmmoChanged();

private:

	struct FCravings
	{
		bool bWantsToRun = false;
		bool bWantsToCrouch = false;
		bool bWantsToAim = false;
	};
	FCravings Cravings;

// States {
	bool RunningIsPossible() const; // minimum conditions for running (regardless of state)

	struct FState
	{
		virtual void Tick(float DeltaTime) {};
		virtual void TakeOver() {};

		APlayerCharacterBase& Character;
		explicit FState(APlayerCharacterBase& Character) : Character(Character) {}
	};

	FState* State = nullptr;
	void SetState(FState& NewState) { State = &NewState; State->TakeOver(); }

	struct FState_UprightNotRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_UprightNotRunning(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_UprightNotRunning State_UprightNotRunning{*this};

	struct FState_UprightRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_UprightRunning(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_UprightRunning State_UprightRunning{*this};

	struct FState_Crouch : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Crouch(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_Crouch State_Crouch{*this};

	struct FState_TransitionUprightNotRunningToCrouch : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_TransitionUprightNotRunningToCrouch(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_TransitionUprightNotRunningToCrouch State_TransitionUprightNotRunningToCrouch{*this};

	struct FState_TransitionCrouchToUprightNotRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_TransitionCrouchToUprightNotRunning(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_TransitionCrouchToUprightNotRunning State_TransitionCrouchToUprightNotRunning{*this};

	// Aim states -------------------------------------------------------------------------------------------------
	FState* AimState = nullptr;
	void SetAimState(FState& NewState) { AimState = &NewState; AimState->TakeOver(); }

	struct FState_Aim_NoAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_NoAim(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_NoAim State_Aim_NoAim{*this};

	struct FState_Aim_Aim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_Aim(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_Aim State_Aim_Aim{*this};

	struct FState_Aim_TransitionNoAimToAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_TransitionNoAimToAim(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_TransitionNoAimToAim State_Aim_TransitionNoAimToAim{*this};

	struct FState_Aim_TransitionAimToNoAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_TransitionAimToNoAim(APlayerCharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_TransitionAimToNoAim State_Aim_TransitionAimToNoAim{*this};
// } States

// Damage {
	void Die();
	bool bIsDead = false;

	UFUNCTION()
	void TakeDamageCallback(FName BoneName, float Damage);

	UFUNCTION()
	void TakeMomentumCallback(FName BoneName, FVector ImpactPoint, FVector Momentum);
// } Damage

// Stamina {
	FTimerHandle StaminaDecreasingTimerHandle{};
	FTimerHandle StaminaRegenerationTimerHandle{};
	void WaitForSomeTimeAndStartRegeneratingStaminaIfNeeded(int32 OldStamina, int32 NewStamina);
// } Stamina

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

	void OnWeapon1Pressed();
	void OnWeapon2Pressed();
	void OnWeapon3Pressed();
	void OnWeapon4Pressed();
	void OnWeapon5Pressed();
	void OnWeapon6Pressed();
	void OnWeapon7Pressed();
	void OnWeapon8Pressed();
	void OnWeapon9Pressed();
	void OnWeapon0Pressed();
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

// Transition Updater {
	enum class ETransitionFinished { No, Yes };

	struct FSmoothStateTransitionUpdater
	{
		float TransitionTime = 0.f;
		std::function<void(float Coef)> Callback;

		[[nodiscard]] ETransitionFinished Update(float DeltaTime);
	private:
		float TimeInTransition = 0.f;
	};
// } Transition Updater

// Upright to crouch smooth transition {
	float CapsuleUprightHalfHeightBackup = 0.f;
	float SpringArmSocketOffsetZBackup = 0.f;
	float MeshZOffsetFromCapsuleLowestPointBackup = 0.f;

	// coef == 1 for upright, 0 for crouching
	void SetSpringArmRelativeZ(float coef);
	void SetCapsuleHalfHeight(float coef);
	void SetVelocityAccordingToCrouch(float coef);

	FSmoothStateTransitionUpdater UprightToCrouchUpdater;
// } Upright to crouch smooth transition

// Aim to no aim smooth transition {
	FVector2D SpringArmSocketOffsetXYBackup{0.f, 0.f};
	float FovBackup = 0.f;

	FRotator AimRotationCurrent{ 0.f, 0.f, 0.f };
	float AimMouseSensitivityCoef = 1.f;

	// coef == 1 for aim, 0 for no aim
	void SetSpringArmRelativeXY(float coef);
	void SetAimRotation(float coef);
	void SetFov(float coef);

	FSmoothStateTransitionUpdater AimToNoAimUpdater;
// } Upright to crouch smooth transition

// Upright to crouch switch {
	float CrouchCoef = 0.f;

	bool CanStandUpright() const;
// } Upright to crouch switch

// Attack {
	void BeginAttack();
	void EndAttack();

	bool CanMakeMeleeAttack() const;
	bool CanMakeRangedAttack() const;

	void BeginAim();
	void EndAim();

	void OnMeleeAttackFinished();
	void OnRangedAttackFinished();
// } Attack

// SmoothlyOrientSelfToWorldYawValue facilities {
	void RotateSelfIfNeeded(float DeltaTime);

	bool bSmoothlyOrientSelf_Required = false;
	float SmoothlyOrientSelf_WorldYawValue = 0.f;
// } SmoothlyOrientSelfToWorldYawValue facilities

	UPROPERTY()
	class UCharacterManHUDWidget* HUDWidget = nullptr;

	void SetMovementSettingsForRunning();
	void SetMovementSettingsForNotRunning();
	void SetMovementSettingsForNotRunningImpl();
	FTimerHandle MovementSettingsForRunningTimerHandle;
};
