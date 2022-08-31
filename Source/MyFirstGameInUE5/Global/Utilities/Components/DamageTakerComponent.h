#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DamageTakerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamageMulticastDynamicSignature, FName, BoneName, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMomentumMulticastDynamicSignature, FName, BoneName, FVector, ImpactPoint, FVector, Momentum);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UDamageTakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageTakerComponent() { PrimaryComponentTick.bCanEverTick = false; }

	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FDamageMulticastDynamicSignature DamageTaken;

	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FMomentumMulticastDynamicSignature MomentumTaken;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void InflictDamage(AActor* TargetActor, FName BoneName, float Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void GiveMomentum(AActor* TargetActor, FName BoneName, FVector ImpactPoint, FVector Momentum);

private:
	static UDamageTakerComponent* FindDamageTakerComponent(AActor* TargetActor);
};
