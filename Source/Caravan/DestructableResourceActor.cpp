#include "DestructableResourceActor.h"

#include "Caravan.h"
#include "Components/InteractableComponent.h"
#include "CraftResourceActor.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "RPG/InventoryFunctionLibrary.h"
#include "RPG/PickupItemActor.h"
#include "Utils/CaravanEngineUtils.h"
#include "WorldBuilder/WorldBuilderSubsystem.h"
#include "WorldBuilder/WorldGenerationSpec.h"

ADestructableResourceActor::ADestructableResourceActor(const class FObjectInitializer& ObjInitializer)
	: ADestructableResourceActor(ObjInitializer, ECraftResourceType::Invalid)
{}

ADestructableResourceActor::ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType)
	: Super(ObjInitializer)
	, ResourceType(resourceType)
	, ResourceDropCount(1)
	, Health(100.f)
{
	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("TreeActor_StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->SetupAttachment(RootComponent);
	}
}

void ADestructableResourceActor::BeginPlay()
{
	Super::BeginPlay();

	// Event Registration
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ADestructableResourceActor::OnInteract);
	}
}

void ADestructableResourceActor::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	Health -= 35; // TODO: From weapon controller
	if (Health <= 0)
	{
		// Drop craft resource actorsw
		UWorldGenerationSpec* WorldSpec = NULL;
		if (UWorldBuilderSubsystem* WorldBuilderSubsystem = GetGameInstance()->GetSubsystem<UWorldBuilderSubsystem>())
		{
			for (int i = 0; i < ResourceDropCount; ++i)
			{
				// TODO : Do this better
				FName ItemName;
				switch (ResourceType)
				{
				case ECraftResourceType::Wood: ItemName = TEXT("Wood"); break;
				case ECraftResourceType::Stone: ItemName = TEXT("Stone"); break;
				}

				const FDataTableRowHandle ItemHandle = WorldBuilderSubsystem->GetResourceItemHandle(ItemName);
				SpawnResourceActor(ItemHandle, i);
			}
		}

		Destroy();
	}
}

APickupItemActor* ADestructableResourceActor::SpawnResourceActor(const FDataTableRowHandle& ItemHandle, int ActorIndex)
{
	if (!ensureMsgf(!ItemHandle.IsNull(), TEXT("[ADestructableResourceActor::SpawnResourceActor] Invalid ItemHandle!")))
		return nullptr;

	static float LOCATION_OFFSET_LENGTH = 100.f;

	APickupItemActor* ResourceActor = UInventoryFunctionLibrary::SpawnPickupItem(GetWorld(), ItemHandle, FTransform::Identity);
	if(ResourceActor)
	{
		const FBox LocalBB = GetComponentsBoundingBox();
		const FBox ResourceBB = ResourceActor->GetComponentsBoundingBox();

		// Offset position along forward, rotated around up by a fixed amount interval relative to amount of wood dropped
		const FVector LocationOffset = GetActorForwardVector().RotateAngleAxis(ActorIndex * 360.f / (float)ResourceDropCount, GetActorUpVector());
		FVector Location = LocalBB.GetCenter() + (LocationOffset * LOCATION_OFFSET_LENGTH);

		// Offset position along up by random range (-n < x < n)
		float ZOffset = FMath::Max(0.f, LocalBB.GetExtent().Z - (ResourceBB.GetExtent().Z * 1.5f));
		Location += GetActorUpVector() * FMath::RandRange(-1.f, 0.f) * ZOffset;
		ResourceActor->SetActorLocation(Location);

		// Rotate slightly around pitch and yaw
		const FRotator Rotation = FRotator(FMath::RandRange(5, 25), FMath::RandRange(30, 330), 0.f);
		ResourceActor->SetActorRotation(Rotation);
	}
	return ResourceActor;
}
