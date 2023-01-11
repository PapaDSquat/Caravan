#include "RPG/InventoryFunctionLibrary.h"

#include "RPG/InventoryItemData.h"
#include "RPG/PickupItemActor.h"

APickupItemActor* UInventoryFunctionLibrary::SpawnPickupItem(UWorld* World, const FDataTableRowHandle& ItemHandle, const FTransform& Transform)
{
	FInventoryItemDataRow ItemDef;
	if (!GetItemDefinition(ItemHandle, ItemDef))
	{
		// TODO : Error
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	APickupItemActor* SpawnedActor = World->SpawnActor<APickupItemActor>(Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParameters);
	if (SpawnedActor)
	{
		SpawnedActor->InitializeItem(ItemHandle, ItemDef);
	}
	return SpawnedActor;
}

bool UInventoryFunctionLibrary::GetItemDefinition(const FDataTableRowHandle& ItemHandle, FInventoryItemDataRow& OutItemDef)
{
	if (const FInventoryItemDataRow* ItemDef = ItemHandle.GetRow<FInventoryItemDataRow>(TEXT("GetItemDefinition"));
		ItemDef != nullptr)
	{
		OutItemDef = *ItemDef;
		return true;
	}
	return false;
}