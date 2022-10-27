#include "Interaction/Pickups/HealingPickup.h"

#include "global/Utilities/Components/ClampedIntegerComponent.h"

#include "GameFramework/Pawn.h"

FText AHealingPickup::FocusText_Implementation()
{
	return FText::Format(FTextFormat(NSLOCTEXT("Interaction", "PickHealth", "Heal self by {0}")), Amount);
}

void AHealingPickup::Interact_Implementation(class APawn* Pawn)
{
	auto HealthComponent = Cast<UClampedIntegerComponent>(Pawn->GetDefaultSubobjectByName(HealthComponentName));
	if (HealthComponent == nullptr)
	{
		return;
	}
	int32 AmountNeeded = HealthComponent->Max - HealthComponent->GetValue();
	if (AmountNeeded == 0)
	{
		return;
	}
	HealthComponent->Increase(Amount);
	Amount -= AmountNeeded;
	if (Amount <= 0)
	{
		Destroy();
	}
}
