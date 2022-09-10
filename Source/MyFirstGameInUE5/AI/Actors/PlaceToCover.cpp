#include "AI/Actors/PlaceToCover.h"

#include "Components/ArrowComponent.h"

APlaceToCover::APlaceToCover()
{
	PrimaryActorTick.bCanEverTick = false;

	CoverPositionRange1 = CreateDefaultSubobject<UArrowComponent>("CoverPositionRange1");
	SetRootComponent(CoverPositionRange1);
	CoverPositionRange2 = CreateDefaultSubobject<UArrowComponent>("CoverPositionRange2");
	CoverPositionRange2->SetupAttachment(GetRootComponent());
}

FVector APlaceToCover::NearestPosInCover(FVector ToWhat) const
{
	auto P1 = CoverPositionRange1->GetComponentLocation();
	auto P2 = CoverPositionRange2->GetComponentLocation();
	auto N = (P2 - P1);
	auto L = N.Length();
	if (FMath::IsNearlyZero(L))
	{
		return (P1 + P2) / 2.f;
	}
	N /= L;
	auto M = FVector::DotProduct(ToWhat - P1, N);
	if (M < 0)
	{
		return P1;
	}
	else if (M > L)
	{
		return P2;
	}
	else
	{
		return P1 + M * N;
	}
}

FVector APlaceToCover::OrientationInPos(FVector Pos) const
{
	auto P1 = CoverPositionRange1->GetComponentLocation();
	auto P2 = CoverPositionRange2->GetComponentLocation();
	auto N = (P2 - P1);
	auto L = N.Length();
	if (FMath::IsNearlyZero(L))
	{
		return (P1 + P2) / 2.f;
	}
	N /= L;
	auto M = FVector::DotProduct(Pos - P1, N);

	auto Coef = M / L;
	return (CoverPositionRange1->GetForwardVector() * (1 - Coef) + CoverPositionRange2->GetForwardVector() * Coef).GetSafeNormal();
}

FVector APlaceToCover::GetCenter() const
{
	return (CoverPositionRange1->GetComponentLocation() + CoverPositionRange2->GetComponentLocation()) / 2.f;
}
