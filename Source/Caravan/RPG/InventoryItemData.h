#pragma once

#include "CoreMinimal.h"
#include "CraftResourceActor.h" // For ResourceType
#include "Engine/DataTable.h"
#include "InventoryItemData.generated.h"

USTRUCT(BlueprintType)
struct CARAVAN_API FInventoryItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// TODO : Change to FGameplayTag, or remove?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> UIIcon;
};

USTRUCT(BlueprintType)
struct CARAVAN_API FItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle ItemHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 0;
};