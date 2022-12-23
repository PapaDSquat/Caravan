#include "CampBuildingActor.h"

#include "Caravan.h"
#include "Components/InteractableComponent.h"

ACampBuildingActor::ACampBuildingActor()
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACampBuildingActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetBuildingState(IsConstructed() ? ECampBuildingState::Constructed : ECampBuildingState::Deconstructed);
}

void ACampBuildingActor::SetActive(bool bValue)
{
	if (bActive != bValue)
	{
		bActive = bValue;

		SetActorHiddenInGame(!bActive);
		InteractableComponent->SetActive(bActive);

		if (!bValue)
		{
			DeconstructBuilding();
		}
	}
}

void ACampBuildingActor::ConstructBuilding(float PercentAmount)
{
	if (!ensureMsgf(PercentAmount > 0.f, TEXT("[ACampBuildingActor::ConstructBuilding] Percent amount cannot be a negative number")))
		return;

	if (IsConstructed())
		return;

	ConstructedPercent = FMath::Min(ConstructedPercent + PercentAmount, 1.f);

	OnBuildingConstructProgress(ConstructedPercent);

	if (ConstructedPercent == 1.f)
	{
		SetBuildingState(ECampBuildingState::Constructed);
		OnBuildingConstructed();
	}
}

void ACampBuildingActor::DeconstructBuilding()
{
	ConstructedPercent = 0.f; // Reset construction progress
	SetBuildingState(ECampBuildingState::Deconstructed);
}

void ACampBuildingActor::SetBuildingState(ECampBuildingState InState)
{
	if (BuildingState != InState)
	{
		const ECampBuildingState OldState = BuildingState;
		BuildingState = InState;

		OnBuildingStateChange(OldState, BuildingState);
	}
}
