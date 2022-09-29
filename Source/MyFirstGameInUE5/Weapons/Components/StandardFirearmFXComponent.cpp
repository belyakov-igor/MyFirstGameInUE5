#include "Weapons/Components/StandardFirearmFXComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"

UStandardFirearmFXComponent::UStandardFirearmFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStandardFirearmFXComponent::PlayFX(FHitResult Hit, USkeletalMeshComponent* WeaponMesh, FName MuzzleSocketName)
{
    if (MuzzleFlashNiagaraEffect != nullptr)
    {
        check(WeaponMesh != nullptr);
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlashNiagaraEffect
            , WeaponMesh
            , MuzzleSocketName
            , FVector::ZeroVector
            , FRotator::ZeroRotator
            , EAttachLocation::SnapToTarget
            , /* bAutoDestroy */ true
        );
    }

    if (ShotSound != nullptr)
    {
        check(WeaponMesh != nullptr);
        UGameplayStatics::SpawnSoundAttached(ShotSound, WeaponMesh, MuzzleSocketName);
    }

    if (BulletTraceNiagaraEffect != nullptr, WeaponMesh)
    {
        const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld()
            , BulletTraceNiagaraEffect
            , WeaponMesh->GetSocketTransform(MuzzleSocketName).GetLocation()
        );
        if (TraceFXComponent != nullptr)
        {
            TraceFXComponent->SetNiagaraVariableVec3(TraceEndVariableName, Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd);
        }
    }

    auto SurfaceImpactData = DefaultSurfaceImpactData;
    if (Hit.PhysMaterial.IsValid())
    {
        if (auto SurfaceImpactDataPtr = SurfaceImpactDataMap.Find(Hit.PhysMaterial.Get()); SurfaceImpactDataPtr != nullptr)
        {
            SurfaceImpactData = *SurfaceImpactDataPtr;
        }
    }

    if (SurfaceImpactData.NiagaraEffect != nullptr)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld()
            , SurfaceImpactData.NiagaraEffect
            , Hit.ImpactPoint
            , Hit.ImpactNormal.Rotation()
        );
    }

    if (SurfaceImpactData.DecalData.Material != nullptr)
    {
        auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
            GetWorld()
            , SurfaceImpactData.DecalData.Material
            , SurfaceImpactData.DecalData.Size
            , Hit.ImpactPoint
            , Hit.ImpactNormal.Rotation()
        );
        if (DecalComponent != nullptr)
        {
            DecalComponent->SetFadeOut(SurfaceImpactData.DecalData.LifeTime, SurfaceImpactData.DecalData.FadeoutTime);
        }
    }

    if (SurfaceImpactData.ImpactSound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SurfaceImpactData.ImpactSound, Hit.ImpactPoint);
    }
}
