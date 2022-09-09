#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "CharacterBase.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 10.f, ClampMax = 100.f))
	float CapsuleCrouchHalfHeight = 58.f;

	// How much time takes transitin from upright to crouch and vice-versa
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 0.f, ClampMax = 3.f))
	float UprightToCrouchTransitionTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 0.f))
	float DelayForSetMovementSettingsForNotRunning = 1.0f;

	// How much time takes transition from no aim to aim and vice-versa
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
	TSubclassOf<class ABaseWeapon> DefaultMeleeWeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	TEnumAsByte<ECharacterAnimationSet> AnimationSet = ECharacterAnimationSet::Unarmed;

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

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnWeaponAndAmmoChanged();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attack")
	virtual FRotator GetAimRotation() const { checkNoEntry(); return FRotator::ZeroRotator; }

	struct FCravings
	{
		bool bWantsToRun = false;
		bool bWantsToCrouch = false;
		bool bWantsToAim = false;
	};
	FCravings Cravings;

private:

// States {
	bool RunningIsPossible() const; // minimum conditions for running (regardless of state)

	struct FState
	{
		virtual void Tick(float DeltaTime) {};
		virtual void TakeOver() {};

		ACharacterBase& Character;
		explicit FState(ACharacterBase& Character) : Character(Character) {}
	};

	FState* State = nullptr;
	void SetState(FState& NewState) { State = &NewState; State->TakeOver(); }

	struct FState_UprightNotRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_UprightNotRunning(ACharacterBase& Character) : FState(Character) {}
	};
	FState_UprightNotRunning State_UprightNotRunning{*this};

	struct FState_UprightRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_UprightRunning(ACharacterBase& Character) : FState(Character) {}
	};
	FState_UprightRunning State_UprightRunning{*this};

	struct FState_Crouch : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Crouch(ACharacterBase& Character) : FState(Character) {}
	};
	FState_Crouch State_Crouch{*this};

	struct FState_TransitionUprightNotRunningToCrouch : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_TransitionUprightNotRunningToCrouch(ACharacterBase& Character) : FState(Character) {}
	};
	FState_TransitionUprightNotRunningToCrouch State_TransitionUprightNotRunningToCrouch{*this};

	struct FState_TransitionCrouchToUprightNotRunning : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_TransitionCrouchToUprightNotRunning(ACharacterBase& Character) : FState(Character) {}
	};
	FState_TransitionCrouchToUprightNotRunning State_TransitionCrouchToUprightNotRunning{*this};

	// Aim states -------------------------------------------------------------------------------------------------
	FState* AimState = nullptr;
	void SetAimState(FState& NewState) { AimState = &NewState; AimState->TakeOver(); }

	struct FState_Aim_NoAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_NoAim(ACharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_NoAim State_Aim_NoAim{*this};

	struct FState_Aim_Aim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_Aim(ACharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_Aim State_Aim_Aim{*this};

	struct FState_Aim_TransitionNoAimToAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_TransitionNoAimToAim(ACharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_TransitionNoAimToAim State_Aim_TransitionNoAimToAim{*this};

	struct FState_Aim_TransitionAimToNoAim : FState
	{
		virtual void Tick(float DeltaTime) override;
		virtual void TakeOver() override;
		explicit FState_Aim_TransitionAimToNoAim(ACharacterBase& Character) : FState(Character) {}
	};
	FState_Aim_TransitionAimToNoAim State_Aim_TransitionAimToNoAim{*this};
// } States

// SmoothlyOrientSelfToWorldYawValue {
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion")
	float InterpSpeedToSmoothlyOrientSelf = 5.f;
private:
	void SmoothlyOrientSelfToWorldYawValue(float WorldYawValue);

	void StopSmoothlyOrientSelfToWorldYawValue();

	void RotateSelfIfNeeded(float DeltaTime);

	bool bSmoothlyOrientSelf_Required = false;
	float SmoothlyOrientSelf_WorldYawValue = 0.f;
// } SmoothlyOrientSelfToWorldYawValue

// Damage {
protected:
	virtual void Die();
	bool bIsDead = false;

private:
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

	// Coef == 1 for upright, 0 for crouching
	void SetCapsuleHalfHeight(float Coef);
	void SetVelocityAccordingToCrouch(float Coef);

	FSmoothStateTransitionUpdater UprightToCrouchUpdater;

protected:
	virtual void UprightToCrouchTransitionCallback(float Coef);
private:
// } Upright to crouch smooth transition

// Aim to no aim smooth transition {
	FVector2D SpringArmSocketOffsetXYBackup{0.f, 0.f};
	float FovBackup = 0.f;

	FRotator AimRotationCurrent{ 0.f, 0.f, 0.f };
	float AimMouseSensitivityCoef = 1.f;

	// Coef == 1 for aim, 0 for no aim
	void SetAimRotation(float Coef);

	FSmoothStateTransitionUpdater AimToNoAimUpdater;

protected:
	virtual void AimToNoAimTransitionCallback(float Coef);
private:
// } Upright to crouch smooth transition

// Upright to crouch switch {
	float CrouchCoef = 0.f;

	bool CanStandUpright() const;
// } Upright to crouch switch

// Attack {
public:
	void BeginAttack();
	void EndAttack();

	void BeginAim();
	void EndAim();

private:
	bool CanMakeMeleeAttack() const;
	bool CanMakeRangedAttack() const;

	void OnMeleeAttackFinished();
	void OnRangedAttackFinished();
// } Attack

	void SetMovementSettingsForRunning();
	void SetMovementSettingsForNotRunning();
	void SetMovementSettingsForNotRunningImpl();
	FTimerHandle MovementSettingsForRunningTimerHandle;
};
