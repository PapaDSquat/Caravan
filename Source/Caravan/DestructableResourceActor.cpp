// Fill out your copyright notice in the Description page of Project Settings.

#include "DestructableResourceActor.h"
#include "Caravan.h"
#include "CraftResourceActor.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

ADestructableResourceActor::ADestructableResourceActor(const class FObjectInitializer& ObjInitializer)
	: ADestructableResourceActor(ObjInitializer, ECraftResourceType::Invalid)
{}

ADestructableResourceActor::ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType)
	: ResourceType(resourceType)
	, ResourceDropCount(1)
	, Health(100.f)
{
	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("TreeActor_StaticMeshComponent"));
}

void ADestructableResourceActor::OnInteractFocus(const InteractData& interactData)
{
	AInteractableActor::OnInteractFocus(interactData);
}

EInteractionType ADestructableResourceActor::OnInteractSelect(const InteractData& interactData)
{
	AInteractableActor::OnInteractSelect(interactData);

	Health -= 35; // TODO: From weapon controller
	if (Health <= 0)
	{
		// Drop craft resource actors
		static float LOCATION_OFFSET_LENGTH = 100.f;

		FBox thisBB = GetComponentsBoundingBox();
		FBox resourceBB;
		FVector position, offset;
		FRotator rotation;

		for (int i = 0; i < ResourceDropCount; ++i)
		{
			FActorSpawnParameters SpawnParameters;
			if (ACraftResourceActor* resourceActor = GetWorld()->SpawnActor<ACraftResourceActor>(SpawnParameters))
			{
				resourceBB = resourceActor->GetComponentsBoundingBox();

				// Offset position along forward, rotated around up by a fixed amount interval relative to amount of wood dropped
				offset = GetActorForwardVector().RotateAngleAxis(i * 360.f / (float)ResourceDropCount, GetActorUpVector());
				position = thisBB.GetCenter() + (offset * LOCATION_OFFSET_LENGTH);

				// Offset position along up by random range (-n < x < n)
				float zOffset = FMath::Max(0.f, thisBB.GetExtent().Z - (resourceBB.GetExtent().Z * 1.5f));
				position += GetActorUpVector() * FMath::RandRange(-1.f, 0.f) * zOffset;

				// Rotate slightly around pitch and yaw
				rotation = FRotator(FMath::RandRange(5, 25), FMath::RandRange(30, 330), 0.f);

				SCraftResourceInitData initData;
				initData.Type = ResourceType;
				initData.Location = position;
				initData.Rotation = rotation;

				resourceActor->InitCraftResource(initData);
			}
		}

		Destroy();
	}
	return EInteractionType::ResourceMine;
}