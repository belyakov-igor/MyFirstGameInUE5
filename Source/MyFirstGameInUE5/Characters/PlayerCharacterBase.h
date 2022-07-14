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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (ClampMin = 1.5f, ClampMax = 5.f))
	float RunSpeedCoef = 2.f;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool IsCroaching() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	bool HasAnyMovementInput() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	FVector GetAcceleration() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locomotion")
	float GetBackwardAcceleration() const;

private:
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void LookUp(float Amount);
	void LookRight(float Amount);

	void OnCroachButtonPressed();
	void OnCroachButtonReleased();
	void OnRunButtonPressed();
	void OnRunButtonReleased();

	bool bWantsToCroach = false;
	bool bWantsToRun = false;

	bool HasMoveForwardInput = false;
	bool HasMoveRightInput = false;

	float MinCameraPitchBackup = 0.f;
	float MaxCameraPitchBackup = 0.f;

	FVector Acceleration{0, 0, 0};
	FVector PreviousVelocity{ 0, 0, 0 };

	static constexpr float JogSpeed = 600.f;
	static constexpr float MinCameraPitch = -80.f;
	static constexpr float MaxCameraPitch = 70.f;
};
