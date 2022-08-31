#include "DamageTakerComponent.h"

#include "GameFramework/Character.h"

void UDamageTakerComponent::InflictDamage(AActor* TargetActor, FName BoneName, float Damage)
{
	auto DamageTakerComponent = FindDamageTakerComponent(TargetActor);
	if (DamageTakerComponent == nullptr)
	{
		return;
	}
	DamageTakerComponent->DamageTaken.Broadcast(BoneName, Damage);
}

void UDamageTakerComponent::GiveMomentum(AActor* TargetActor, FName BoneName, FVector ImpactPoint, FVector Momentum)
{
	auto DamageTakerComponent = FindDamageTakerComponent(TargetActor);
	if (DamageTakerComponent == nullptr)
	{
		return;
	}
	DamageTakerComponent->MomentumTaken.Broadcast(BoneName, ImpactPoint, Momentum);
}

UDamageTakerComponent* UDamageTakerComponent::FindDamageTakerComponent(AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return nullptr;
	}
	return Cast<UDamageTakerComponent>(TargetActor->FindComponentByClass(UDamageTakerComponent::StaticClass()));
}
