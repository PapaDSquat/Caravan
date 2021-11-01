// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CraftResourceActor.h"
#include "Inventory.generated.h"

UCLASS(BlueprintType)
class CARAVAN_API UInventory : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResource(const ACraftResourceActor* resourceActor);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResourceByType(ECraftResourceType resourceType);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int GetCraftResourceCount(ECraftResourceType resourceType) const;

private:
	int CraftResourceCount[ECraftResourceType::MAX];
};
