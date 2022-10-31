#include "AI/Actors/PlaceToCover.h"

#include "Components/ArrowComponent.h"

APlaceToCover::APlaceToCover()
{
	PrimaryActorTick.bCanEverTick = false;

	CoverPosition = CreateDefaultSubobject<UArrowComponent>("CoverPosition");
	CoverPosition->ArrowColor = FColor::Blue;
	SetRootComponent(CoverPosition);
}
