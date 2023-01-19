#pragma once

#include "CoreMinimal.h"
#include "CraftResourceActor.h" // For ResourceType
#include "Engine/DataTable.h"

#include "InventoryItemData.generated.h"

USTRUCT(BlueprintType)
struct CARAVAN_API FItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle ItemHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!bInfinite"))
	int Count = 0;

	// TODO : Implement Infinite properly, maybe as -1 count?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bInfinite = false;
};

USTRUCT(BlueprintType)
struct CARAVAN_API FInventoryItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// TODO : Change to FGameplayTag, or remove?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;
	
	// Optional, used by AI to find interactables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECraftResourceType ResourceType = ECraftResourceType::Invalid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	// Priority to sort display in an item list. Lower number is higher priority.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int DisplayPriority = 999;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MeshScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> UIIcon = nullptr;

	// If craftable, the Resource materials necessary to craft this item
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FItemStack> MaterialsToCraft;
};