// Fill out your copyright notice in the Description page of Project Settings.

#include "CaravanBuildingPlatform.h"
#include "Caravan.h"
#include "Components/InteractableComponent.h"

ACaravanBuildingPlatform::ACaravanBuildingPlatform(const class FObjectInitializer& ObjInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CaravanActor_StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	InteractableComponent = FindComponentByClass< UInteractableComponent >();
	if (InteractableComponent == NULL)
	{
		InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
		InteractableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ACaravanBuildingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	SetActive(false);
}

void ACaravanBuildingPlatform::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


void ACaravanBuildingPlatform::SetActive(bool bActive)
{
	if (IsActive != bActive)
	{
		IsActive = bActive;

		//SetActorHiddenInGame(!IsActive);
	}
}