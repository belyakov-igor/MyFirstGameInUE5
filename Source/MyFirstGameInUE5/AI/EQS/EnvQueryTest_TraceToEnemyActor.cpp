#include "AI/EQS/EnvQueryTest_TraceToEnemyActor.h"

#include "Characters/CharacterBase.h"
#include "Controllers/AIControllerBase.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"

UEnvQueryTest_TraceToEnemyActor::UEnvQueryTest_TraceToEnemyActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
	Context = UEnvQueryContext_Querier::StaticClass();
}

void UEnvQueryTest_TraceToEnemyActor::RunTest(FEnvQueryInstance& QueryInstance) const
{
	bool bWantsHit = BoolValue.GetValue();

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(EnvQueryTrace), /*bTraceComplex*/ false);
	TArray<AActor*> Actors;
	if (QueryInstance.PrepareContext(Context, Actors))
	{
		auto IgnoredActors = Actors;
		IgnoredActors.Add(Cast<AActor>(QueryInstance.Owner));
		TraceParams.AddIgnoredActors(IgnoredActors);
	}
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		FVector ItemAimingLocation = GetItemLocation(QueryInstance, It.GetIndex()) + FVector(0.f, 0.f, 10.f);
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult
			, ItemAimingLocation
			, ItemAimingLocation - FVector(0.f, 0.f, 5 * HeightOfTraceStart)
			, ECC_Visibility
			, TraceParams
		);
		if (!bHit)
		{
			check(false);
			It.SetScore(TestPurpose, FilterType, false, bWantsHit);
			continue;
		}
		ItemAimingLocation = HitResult.ImpactPoint + FVector(0.f, 0.f, HeightOfTraceStart);
		for (auto Actor : Actors)
		{
			const auto TargetCharacter = Cast<ACharacterBase>(Actor);
			if (TargetCharacter == nullptr)
			{
				continue;
			}
			bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult
				, ItemAimingLocation
				, TargetCharacter->GetActorLocation()
				, ECC_Visibility
				, TraceParams
			);
			It.SetScore(TestPurpose, FilterType, bHit, bWantsHit);
		}
	}
}
