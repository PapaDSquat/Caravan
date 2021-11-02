// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftResourceActor.h"
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

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResource(const ACraftResourceActor* resourceActor);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftResourceByType(ECraftResourceType resourceType);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int GetCraftResourceCount(ECraftResourceType resourceType) const;

private:
	int CraftResourceCount[ECraftResourceType::Invalid];
};
