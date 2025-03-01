// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftResourceActor.h"
#include "Engine/DataTable.h"
#include "RPG/InventoryItemData.h"

#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARAVAN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FItemStack>& GetItemStacks() const { return Items; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItems(const FDataTableRowHandle& ItemHandle, int Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemStack(const FItemStack& ItemStack);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItems(const FDataTableRowHandle& ItemHandle, int Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemStack(const FItemStack& ItemStack);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemStacks(const TArray<FItemStack>& ItemStacks);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropAllItems();

	bool HasItems(const FDataTableRowHandle& ItemHandle, int Count) const;
	bool HasItems(const FItemStack& ItemStack) const;
	bool HasItems(const TArray<FItemStack>& ItemStacks) const;

	// TODO : Remove
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResource(const ACraftResourceActor* resourceActor);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResourceByType(ECraftResourceType resourceType);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int GetCraftResourceCount(ECraftResourceType resourceType) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemStack> DefaultItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool bSortItemsByDisplayPriority = true;

private:
	FItemStack* FindItemStack(const FDataTableRowHandle& ItemHandle);
	const FItemStack* FindItemStack(const FDataTableRowHandle& ItemHandle) const;
	int32 FindItemStackIndex(const FDataTableRowHandle& ItemHandle);
	void SortItems();

	UPROPERTY(SaveGame)
	TArray<FItemStack> Items;

	// TODO : remove
	int CraftResourceCount[ECraftResourceType::Invalid];
};
