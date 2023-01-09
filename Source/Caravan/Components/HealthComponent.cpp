#include "Components/HealthComponent.h"

#include "DrawDebugHelpers.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::ApplyDamage(AActor* Instigator, float Damage)
{
	Health = FMath::Max(0.f, Health - Damage);

	OnDamage.Broadcast(this, Instigator, Damage);

	if (FMath::IsNearlyZero(Health))
	{
		Kill(Instigator);
	}
}

void UHealthComponent::Kill(AActor* Instigator)
{
	Health = 0.f;
	
	OnKill.Broadcast(this, Instigator);
}
