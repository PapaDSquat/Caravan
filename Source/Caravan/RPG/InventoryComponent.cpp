// Fill out your copyright notice in the Description page of Project Settings.

#include "RPG/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	// PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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