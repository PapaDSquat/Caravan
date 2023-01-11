#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "InventoryFunctionLibrary.generated.h"

class APickupItemActor;
struct FInventoryItemDataRow;

UCLASS()
class CARAVAN_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static APickupItemActor* SpawnPickupItem(UWorld* World, const FDataTableRowHandle& ItemHandle, const FTransform& Transform);

	// Helper function to access any items definition by the item handle (from the associated Inventory*Item data table)
	UFUNCTION(BlueprintPure)
	static bool GetItemDefinition(const FDataTableRowHandle& ItemHandle, FInventoryItemDataRow& OutItemDef);

};