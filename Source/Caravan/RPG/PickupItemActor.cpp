#include "RPG/PickupItemActor.h"

#include "Caravan.h"
#include "Components/InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "RPG/InventoryComponent.h"
#include "RPG/InventoryFunctionLibrary.h"
#include "Utils/CaravanEngineUtils.h"

// Sets default values
APickupItemActor::APickupItemActor(const class FObjectInitializer& ObjInitializer)
{
	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);

	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->SetupAttachment(RootComponent);
	}
}

void APickupItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &APickupItemActor::OnInteract);
	}
}

void APickupItemActor::InitializeItem(const FDataTableRowHandle& InItemHandle, const FInventoryItemDataRow& InItemDef)
{
	ItemHandle = InItemHandle;

	StaticMeshComponent->SetStaticMesh(InItemDef.StaticMesh);
	StaticMeshComponent->SetWorldScale3D(InItemDef.MeshScale);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->WakeRigidBody();
}

void APickupItemActor::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	if (InteractingPawn != NULL)
	{
		UInventoryComponent* InventoryComponent = nullptr;

		// Always deposit into player inventory
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = Cast<APawn>(PlayerController->GetPawn()))
			{
				InventoryComponent = PlayerPawn->FindComponentByClass<UInventoryComponent>();
			}
		}

		// Enable to use Local inventory
		// InventoryComponent = InteractingPawn->FindComponentByClass<UInventoryComponent>();

		if (InventoryComponent)
		{
			InventoryComponent->AddItems(ItemHandle);
		}
	}

	Destroy();
}
