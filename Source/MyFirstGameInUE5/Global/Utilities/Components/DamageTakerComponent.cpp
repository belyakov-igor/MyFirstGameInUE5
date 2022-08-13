#include "DamageTakerComponent.h"

#include "GameFramework/Character.h"

void UDamageTakerComponent::TakePenetrationDamage(float DamageAmount, EBodyPart BodyPart)
{
	check(GetOwner() != nullptr);
	GetOwner()->TakeDamage(
		BodyPart == EBodyPart::Head ? HeadShotDamageMultiplier * DamageAmount : DamageAmount
		, FDamageEvent{}
		, nullptr
		, nullptr
	);
}

void UDamageTakerComponent::InflictPenetrationDamage(AActor* TargetActor, float DamageAmount, EBodyPart BodyPart)
{
	if (TargetActor == nullptr)
	{
		return;
	}
	auto DamageTakerComponent =
		Cast<UDamageTakerComponent>(TargetActor->FindComponentByClass(UDamageTakerComponent::StaticClass()))
	;
	if (DamageTakerComponent == nullptr)
	{
		return;
	}
	DamageTakerComponent->TakePenetrationDamage(DamageAmount, BodyPart);
}

void UDamageTakerComponent::TakeBluntHitDamage(float DamageAmount, FVector Momentum)
{
	check(GetOwner() != nullptr);
	GetOwner()->TakeDamage(DamageAmount, FDamageEvent{}, nullptr, nullptr);
	if (auto Character = Cast<ACharacter>(GetOwner()); Character != nullptr)
	{
		Character->LaunchCharacter(Momentum / Mass, /*bXYOverride*/ false, /*bZOverride*/ false);
	}
}

void UDamageTakerComponent::InflictBluntHitDamage(AActor* TargetActor, float DamageAmount, FVector Momentum)
{
	if (TargetActor == nullptr)
	{
		return;
	}
	auto DamageTakerComponent =
		Cast<UDamageTakerComponent>(TargetActor->FindComponentByClass(UDamageTakerComponent::StaticClass()))
	;
	if (DamageTakerComponent == nullptr)
	{
		return;
	}
	DamageTakerComponent->TakeBluntHitDamage(DamageAmount, Momentum);
}
