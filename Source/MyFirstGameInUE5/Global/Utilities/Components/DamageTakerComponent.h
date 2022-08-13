#pragma once

#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DamageTakerComponent.generated.h"



UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	  Head     UMETA(DisplayName = "Head")
	, Other    UMETA(DisplayName = "Other")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTGAMEINUE5_API UDamageTakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageTakerComponent() { PrimaryComponentTick.bCanEverTick = false; }

// Penetration { ==========================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float HeadShotDamageMultiplier = 4.f;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakePenetrationDamage(float DamageAmount, EBodyPart BodyPart);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void InflictPenetrationDamage(AActor* TargetActor, float DamageAmount, EBodyPart BodyPart);
// } Penetration ==========================================================================

// BluntHit { =============================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float Mass = 80.f;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeBluntHitDamage(float DamageAmount, FVector Momentum);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void InflictBluntHitDamage(AActor* TargetActor, float DamageAmount, FVector Momentum);
// } BluntHit =============================================================================
};
