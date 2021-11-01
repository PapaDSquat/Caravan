// Fill out your copyright notice in the Description page of Project Settings.


#include "RPG/Inventory.h"

void UInventory::AddCraftResource(const ACraftResourceActor* resourceActor)
{
	if (resourceActor)
	{
		AddCraftResourceByType(resourceActor->GetResourceType());
	}
}

void UInventory::AddCraftResourceByType(ECraftResourceType resourceType)
{
	if (CraftResourceHelpers::IsValidType(resourceType))
	{
		CraftResourceCount[(int)resourceType] = CraftResourceCount[(int)resourceType] + 1;
	}
}

int UInventory::GetCraftResourceCount(ECraftResourceType resourceType) const
{
	return CraftResourceHelpers::IsValidType(resourceType) ? CraftResourceCount[(uint8)resourceType] : 0;
}