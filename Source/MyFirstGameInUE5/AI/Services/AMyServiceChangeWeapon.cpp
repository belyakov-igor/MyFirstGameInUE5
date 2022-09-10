#include "AI/Services/AMyServiceChangeWeapon.h"

#include "Weapons/Components/WeaponManagerComponent.h"
#include "Weapons/Components/AmmoComponent.h"
#include "Global/Utilities/MyUtilities.h"

#include "AIController.h"

UAMyServiceChangeWeapon::UAMyServiceChangeWeapon()
{
	NodeName = "Change Weapon";
}

void UAMyServiceChangeWeapon::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	auto Controller = OwnerComp.GetAIOwner();
	if (Controller != nullptr)
	{
		auto WMComponent = Cast<UWeaponManagerComponent>(Controller->GetPawn()->GetDefaultSubobjectByName(RangedWeaponManagerComponentName));
		if (WMComponent == nullptr)
		{
			return;
		}

		int32 ChosenWeaponSlot = -1;
		float ChosenDPS = 0.f;
		for (int32 Slot = 0; Slot != WMComponent->Weapons.Num(); ++Slot)
		{
			const auto Weapon = WMComponent->Weapons[Slot];
			if (Weapon == nullptr)
			{
				continue;
			}

			auto AmmoComponent = Cast<UAmmoComponent>(Weapon->FindComponentByClass(UAmmoComponent::StaticClass()));
			if (AmmoComponent != nullptr && AmmoComponent->IsEmpty())
			{
				continue;
			}

			if (auto DPS = Weapon->GetDPS(); DPS > ChosenDPS)
			{
				ChosenWeaponSlot = Slot;
				ChosenDPS = DPS;
			}
		}
		if (ChosenWeaponSlot != -1)
		{
			WMComponent->SetCurrentWeaponSlot(ChosenWeaponSlot);
		}
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
