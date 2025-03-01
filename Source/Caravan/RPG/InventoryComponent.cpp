// Fill out your copyright notice in the Description page of Project Settings.

#include "RPG/InventoryComponent.h"
#include "RPG/InventoryFunctionLibrary.h"
#include "RPG/PickupItemActor.h"
#include "Utils/CaravanSpawnUtils.h"

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

void UInventoryComponent::AddItems(const FDataTableRowHandle& ItemHandle, int Count /* = 1 */)
{
	if (FItemStack* ExistingItemStack = FindItemStack(ItemHandle))
	{
		ExistingItemStack->Count += Count;
	}
	else
	{
		FItemStack& NewItemStack = Items.Emplace_GetRef();
		NewItemStack.ItemHandle = ItemHandle;
		NewItemStack.Count = Count;

		SortItems(); // Sort after adding any new stack
	}
}

void UInventoryComponent::AddItemStack(const FItemStack& ItemStack)
{
	AddItems(ItemStack.ItemHandle, ItemStack.Count);
}

bool UInventoryComponent::RemoveItems(const FDataTableRowHandle& ItemHandle, int Count /* = 1 */)
{
	const int32 Index = FindItemStackIndex(ItemHandle);
	if(Index != INDEX_NONE)
	{
		FItemStack& ItemStack = Items[Index];
		ItemStack.Count = FMath::Max(0, ItemStack.Count - Count);

		// Uncomment to remove stack when emptied
		//if (ItemStack.Count == 0)
		//{
		//	Items.RemoveAt(Index);
		//}
		return true;
	}
	return false;
}

bool UInventoryComponent::RemoveItemStack(const FItemStack& ItemStack)
{
	return RemoveItems(ItemStack.ItemHandle, ItemStack.Count);
}

bool UInventoryComponent::RemoveItemStacks(const TArray<FItemStack>& ItemStacks)
{
	bool bRemovedSuccessful = !ItemStacks.IsEmpty();
	for (const FItemStack& ItemStack : ItemStacks)
	{
		bRemovedSuccessful &= RemoveItemStack(ItemStack);
	}
	return bRemovedSuccessful;
}

void UInventoryComponent::DropAllItems()
{
	if (Items.IsEmpty())
		return;

	static float LOCATION_OFFSET_LENGTH = 100.f;

	const AActor* const Owner = GetOwner();
	const FBox OwnerBB = Owner->GetComponentsBoundingBox();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const FVector OwnerUp = Owner->GetActorUpVector();

	// TODO : Offset all stacks together
	for (FItemStack& ItemStack : Items)
	{
		for (int i = 0; i < ItemStack.Count; ++i)
		{
			// Offset position along forward, rotated around up by a fixed amount interval relative to amount of wood dropped
			const FVector Offset = OwnerForward.RotateAngleAxis(i * 360.f / (float)ItemStack.Count, OwnerUp);
			FVector SpawnLocation = OwnerBB.GetCenter() + (Offset * LOCATION_OFFSET_LENGTH);

			// Offset position along up by random range (-n < x < n)
			float ZOffset = FMath::Max(0.f, OwnerBB.GetExtent().Z);
			SpawnLocation += OwnerUp * FMath::RandRange(-1.f, 0.f) * ZOffset;

			const FRotator SpawnRotation = FRotator(FMath::RandRange(5, 25), FMath::RandRange(30, 330), 0.f);

			UInventoryFunctionLibrary::SpawnPickupItem(GetWorld(), ItemStack.ItemHandle, FTransform(SpawnRotation, SpawnLocation));
		}
	}

	Items.Empty();
}

bool UInventoryComponent::HasItems(const FDataTableRowHandle& ItemHandle, int Count) const
{
	if (const FItemStack* OwnedItemStack = FindItemStack(ItemHandle))
	{
		return OwnedItemStack->bInfinite || OwnedItemStack->Count >= Count;
	}
	return false;
}

bool UInventoryComponent::HasItems(const FItemStack& ItemStack) const
{
	return HasItems(ItemStack.ItemHandle, ItemStack.Count);
}

bool UInventoryComponent::HasItems(const TArray<FItemStack>& ItemStacks) const
{
	if (ItemStacks.IsEmpty() || Items.IsEmpty())
	{
		return false;
	}

	for (const FItemStack ItemStack : ItemStacks)
	{
		if (!HasItems(ItemStack))
		{
			return false;
		}
	}
	return true;
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

const FItemStack* UInventoryComponent::FindItemStack(const FDataTableRowHandle& ItemHandle) const
{
	return const_cast<UInventoryComponent*>(this)->FindItemStack(ItemHandle);
}

int32 UInventoryComponent::FindItemStackIndex(const FDataTableRowHandle& ItemHandle)
{
	return Items.IndexOfByPredicate(
		[ItemHandle](const FItemStack& ItemStack)
		{
			return ItemStack.ItemHandle == ItemHandle;
		}
	);
}

void UInventoryComponent::SortItems()
{
	if (!bSortItemsByDisplayPriority)
		return;

	Algo::Sort(Items, [](const FItemStack& First, const FItemStack& Second)
	{
			FInventoryItemDataRow* FirstDef = UInventoryFunctionLibrary::GetItemDefinition(First.ItemHandle);
			FInventoryItemDataRow* SecondDef = UInventoryFunctionLibrary::GetItemDefinition(Second.ItemHandle);
			return FirstDef && SecondDef && FirstDef->DisplayPriority < SecondDef->DisplayPriority;
	});
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
