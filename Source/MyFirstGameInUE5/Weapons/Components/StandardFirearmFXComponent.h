#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "StandardFirearmFXComponent.generated.h"

USTRUCT(BlueprintType)
struct FDecalData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    class UMaterialInterface* Material = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FVector Size{5.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    float LifeTime = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    float FadeoutTime = .7f;
};

USTRUCT(BlueprintType)
struct FSurfaceImpactData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    class UNiagaraSystem* NiagaraEffect = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FDecalData DecalData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    class USoundCue* ImpactSound = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYFIRSTGAMEINUE5_API UStandardFirearmFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStandardFirearmFXComponent();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    class UNiagaraSystem* MuzzleFlashNiagaraEffect = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    class UNiagaraSystem* BulletTraceNiagaraEffect = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    FString TraceEndVariableName = "TraceEnd";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    class USoundCue* ShotSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FSurfaceImpactData DefaultSurfaceImpactData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TMap<class UPhysicalMaterial*, FSurfaceImpactData> SurfaceImpactDataMap;

    void PlayFX(FHitResult Hit, USkeletalMeshComponent* WeaponMesh, FName MuzzleSocketName);
};
