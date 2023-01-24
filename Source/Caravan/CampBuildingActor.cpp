#include "CampBuildingActor.h"

#include "Caravan.h"
#include "Caravan/CampBuildingSpec.h"
#include "Components/InteractableComponent.h"
#include "RPG/InventoryComponent.h"
#include "RPG/InventoryItemData.h"
#include "Rpg/InventoryFunctionLibrary.h"

ACampBuildingActor::ACampBuildingActor()
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	AddOwnedComponent(InventoryComponent);
}

void ACampBuildingActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetBuildingState(IsConstructed() ? ECampBuildingState::Constructed : ECampBuildingState::Deconstructed);

	if (CampBuildingSpec)
	{
		for (const FItemStack ItemStack : CampBuildingSpec->DefaultCraftableItems)
		{
			InventoryComponent->AddItemStack(ItemStack);
		 }
	}
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

bool ACampBuildingActor::CraftItem(APawn* InteractingPawn, int InventoryItemIndex)
{
	if (!IsValid(InteractingPawn))
	{
		return false;
	}

	UInventoryComponent* InteractingPawnInventory = InteractingPawn->FindComponentByClass<UInventoryComponent>();
	if (!InteractingPawnInventory)
	{
		return false;
	}

	const TArray<FItemStack>& InventoryItems = InventoryComponent->GetItemStacks();
	if (InventoryItems.Num() <= InventoryItemIndex)
	{
		return false;
	}

	const FItemStack& ItemStack = InventoryItems[InventoryItemIndex];
	const FInventoryItemDataRow* const ItemDef = UInventoryFunctionLibrary::GetItemDefinition(ItemStack.ItemHandle);
	if (!ItemDef)
	{
		return false;
	}

	// Check if can craft
	const bool bCanCraft = InteractingPawnInventory->HasItems(ItemDef->MaterialsToCraft);
	if (!bCanCraft)
	{
		return false;
	}

	// It's Crafting Time!

	// Remove crafting materials
	InteractingPawnInventory->RemoveItemStacks(ItemDef->MaterialsToCraft);
		
	// Add crafted resource to pawn inventory
	InteractingPawnInventory->AddItems(ItemStack.ItemHandle, 1);

	// Remove from building inventory
	if (!ItemStack.bInfinite)
	{
		InventoryComponent->RemoveItems(ItemStack.ItemHandle, 1);
	}
	return true;
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
