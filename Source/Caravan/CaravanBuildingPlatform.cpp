// Fill out your copyright notice in the Description page of Project Settings.

#include "CaravanBuildingPlatform.h"
#include "Caravan.h"

ACaravanBuildingPlatform::ACaravanBuildingPlatform(const class FObjectInitializer& ObjInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CaravanActor_StaticMeshComponent"));
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