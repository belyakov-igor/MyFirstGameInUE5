#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AmmoComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYFIRSTGAMEINUE5_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UAmmoComponent();

	// Arsenal -----------------------------------------------------------------------
	FInt32Int32MulticastSignature ArsenalChanged;
	FSignalMulticastSignature ArsenalIsFull;
	FSignalMulticastSignature ArsenalIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arsenal")
	int32 ArsenalCapacity = 100;


	UFUNCTION(BlueprintCallable, Category = "Arsenal")
	void MakeArsenalFull() { ArsenalAmmo = ArsenalCapacity; }

	UFUNCTION(BlueprintCallable, Category = "Arsenal")
	void MakeArsenalEmpty() { ArsenalAmmo = 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Arsenal")
	int32 GetArsenalAmount() const { return ArsenalAmmo; }

	// Return amount of ammo that didn't fit into the arsenal
	UFUNCTION(BlueprintCallable, Category = "Arsenal")
	int32 IncreaseArsenal(int32 Amount);

	// Return amount of ammo that couldn't be taken from the the arsenal
	UFUNCTION(BlueprintCallable, Category = "Arsenal")
	int32 DecreaseArsenal(int32 Amount);

	// Clip --------------------------------------------------------------------------
	FInt32Int32MulticastSignature ClipChanged;
	FSignalMulticastSignature ClipIsFull;
	FSignalMulticastSignature ClipIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clip")
	int32 ClipCapacity = 10;


	UFUNCTION(BlueprintCallable, Category = "Clip")
	void MakeClipFull() { ClipAmmo = ClipCapacity; }

	UFUNCTION(BlueprintCallable, Category = "Clip")
	void MakeClipEmpty() { ClipAmmo = 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Clip")
	int32 GetClipAmount() const { return ClipAmmo; }

	// Return amount of ammo that didn't fit into the clip
	UFUNCTION(BlueprintCallable, Category = "Clip")
	int32 IncreaseClip(int32 Amount);

	// Return amount of ammo that couldn't be taken from the the clip
	UFUNCTION(BlueprintCallable, Category = "Clip")
	int32 DecreaseClip(int32 Amount);

	// General -----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "General")
	class UAnimMontage* ReloadAnimMontage;
	
	// Discard current clip and set a new one with maximum possible ammo
	UFUNCTION(BlueprintCallable, Category = "General")
	void Reload();

	void ChangeClip();

private:
	int32 ArsenalAmmo;
	int32 ClipAmmo;

	int32 Increase(
		int32 Amount
		, int32& Ammo
		, int32 Capacity
		, FSignalMulticastSignature& ReachedMax
		, FInt32Int32MulticastSignature& AmountChanged
	);
	int32 Decrease(
		int32 Amount
		, int32& Ammo
		, FSignalMulticastSignature& ReachedMin
		, FInt32Int32MulticastSignature& AmountChanged
	);

	void OnReloadAnimFinished(class USkeletalMeshComponent* Mesh);
};
