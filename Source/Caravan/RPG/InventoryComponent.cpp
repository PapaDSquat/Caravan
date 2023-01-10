// Fill out your copyright notice in the Description page of Project Settings.

#include "RPG/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	// PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Add default items to inventory
	// TODO : Won't work properly with save game - we should only do this when first ever spawning a character
	if (Items.IsEmpty() && !DefaultItems.IsEmpty())
	{
		for (const FItemStack& ItemStack : DefaultItems)
		{
			AddItemStack(ItemStack);
		}
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::AddItems(const FDataTableRowHandle& ItemHandle, int Count /* = 0 */)
{
	if (FItemStack* ExistingItemStack = FindItemStack(ItemHandle))
	{
		ExistingItemStack->Count += Count;
	}
	else
	{
		FItemStack NewItemStack = Items.Emplace_GetRef();
		NewItemStack.ItemHandle = ItemHandle;
		NewItemStack.Count = Count;
	}
}

void UInventoryComponent::AddItemStack(const FItemStack& ItemStack)
{
	AddItems(ItemStack.ItemHandle, ItemStack.Count);
}

FItemStack* UInventoryComponent::FindItemStack(const FDataTableRowHandle& ItemHandle)
{
	return Items.FindByPredicate(
		[ItemHandle](const FItemStack& ItemStack)
		{
			return ItemStack.ItemHandle == ItemHandle;
		}
	);
}

void UInventoryComponent::AddCraftResource(const ACraftResourceActor* resourceActor)
{
	if (resourceActor)
	{
		AddCraftResourceByType(resourceActor->GetResourceType());
	}
}

void UInventoryComponent::AddCraftResourceByType(ECraftResourceType resourceType)
{
	if (CraftResourceHelpers::IsValidType(resourceType))
	{
		CraftResourceCount[(int)resourceType] = CraftResourceCount[(int)resourceType] + 1;
	}
}

int UInventoryComponent::GetCraftResourceCount(ECraftResourceType resourceType) const
{
	return CraftResourceHelpers::IsValidType(resourceType) ? CraftResourceCount[(uint8)resourceType] : 0;
}
