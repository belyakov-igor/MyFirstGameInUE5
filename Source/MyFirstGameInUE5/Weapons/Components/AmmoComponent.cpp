#include "Weapons/Components/AmmoComponent.h"

#include "Global/Utilities/MyUtilities.h"
#include "Weapons/Actors/BaseWeapon.h"
#include "Weapons/Components/WeaponManagerComponent.h"
#include "Animation/ReloadFinishedAnimNotify.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

	auto ReloadFinishedNotify = UMyUtilities::FindNotifyByClass<UReloadFinishedAnimNotify>(ReloadAnimMontage);
	ReloadFinishedNotify->OnNotified.AddUObject(this, &UAmmoComponent::OnReloadAnimFinished);
}

int32 UAmmoComponent::Increase(
	int32 Amount
	, int32& Ammo
	, int32 Capacity
	, FSignalMulticastSignature& IsFull
	, FInt32Int32MulticastSignature& Changed
)
{
	checkf(Amount >= 0, TEXT("Amount must be non-negative"));
	int32 Ret = 0;
	if (Amount == 0)
	{
		return Ret;
	}
	if (Ammo == Capacity) // nothing changed, no broadcast
	{
		Ret = Amount;
		return Ret;
	}
	auto OldAmmo = Ammo;
	Ammo += Amount;
	if (Ammo >= Capacity)
	{
		Ret = Ammo - Capacity;
		Ammo = Capacity;
		IsFull.Broadcast();
	}
	Changed.Broadcast(OldAmmo, Ammo);
	return Ret;
}

int32 UAmmoComponent::Decrease(
	int32 Amount
	, int32& Ammo
	, FSignalMulticastSignature& IsEmpty
	, FInt32Int32MulticastSignature& Changed
)
{
	checkf(Amount >= 0, TEXT("Amount must be non-negative"));
	int32 Ret = 0;
	if (Amount == 0)
	{
		return Ret;
	}
	if (Ammo == 0) // nothing changed, no broadcast
	{
		Ret = Amount;
		return Ret;
	}
	auto OldAmmo = Ammo;
	Ammo -= Amount;
	if (Ammo <= 0)
	{
		Ret = -Ammo;
		Ammo = 0;
		IsEmpty.Broadcast();
	}
	Changed.Broadcast(OldAmmo, Ammo);
	return Ret;
}

int32 UAmmoComponent::IncreaseArsenal(int32 Amount)
{
	return Increase(Amount, ArsenalAmmo, ArsenalCapacity, ArsenalIsFull, ArsenalChanged);
}

int32 UAmmoComponent::DecreaseArsenal(int32 Amount)
{
	return Decrease(Amount, ArsenalAmmo, ArsenalIsEmpty, ArsenalChanged);
}


int32 UAmmoComponent::IncreaseClip(int32 Amount)
{
	return Increase(Amount, ClipAmmo, ClipCapacity, ClipIsFull, ClipChanged);
}

int32 UAmmoComponent::DecreaseClip(int32 Amount)
{
	return Decrease(Amount, ClipAmmo, ClipIsEmpty, ClipChanged);
}

void UAmmoComponent::Reload()
{
	if (ArsenalAmmo == 0)
	{
		return;
	}
	if (ReloadAnimMontage == nullptr)
	{
		checkf(false, TEXT("Reload anim montage should be set."));
		return;
	}

	auto Weapon = Cast<ABaseWeapon>(GetOwner());
	check(Weapon != nullptr);
	auto Character = Cast<ACharacter>(Weapon->GetOwner());
	check(Character != nullptr);

	UGameplayStatics::SpawnSoundAttached(Weapon->ReloadSound, Weapon->WeaponMesh, Weapon->HandGripSocketName);
	Character->PlayAnimMontage(ReloadAnimMontage);
}

void UAmmoComponent::OnReloadAnimFinished(USkeletalMeshComponent* Mesh)
{
	auto Weapon = Cast<ABaseWeapon>(GetOwner());
	check(Weapon != nullptr);
	auto Character = Cast<ACharacter>(Weapon->GetOwner());
	check(Character != nullptr);
	if (Mesh != Character->GetMesh())
	{
		return;
	}
	bool IsThisWeapon = false;
	TArray<UActorComponent*> WeaponManagerComponents;
	Character->GetComponents(UWeaponManagerComponent::StaticClass(), WeaponManagerComponents);
	for (auto WeaponManagerComponent_ : WeaponManagerComponents)
	{
		auto WeaponManagerComponent = Cast<UWeaponManagerComponent>(WeaponManagerComponent_);
		check(WeaponManagerComponent != nullptr);
		if (WeaponManagerComponent->GetCurrentWeapon() == Weapon)
		{
			IsThisWeapon = true;
		}
	}
	if (!IsThisWeapon)
	{
		return;
	}
	ChangeClip();
	Character->StopAnimMontage(ReloadAnimMontage);
}

void UAmmoComponent::ChangeClip()
{
	auto OldClipAmmo = ClipAmmo;
	auto OldArsenalAmmo = ArsenalAmmo;
	ClipAmmo = FMath::Min(ClipCapacity, ArsenalAmmo);
	ArsenalAmmo -= ClipAmmo;

	ArsenalChanged.Broadcast(OldArsenalAmmo, ArsenalAmmo);
	ClipChanged.Broadcast(OldClipAmmo, ClipAmmo);
	if (ArsenalAmmo == 0)
	{
		ArsenalIsEmpty.Broadcast();
	}
	if (ClipAmmo == ClipCapacity)
	{
		ClipIsFull.Broadcast();
	}
}
