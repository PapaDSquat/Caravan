// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "CraftResourceActor.h"
#include "DestructableResourceActor.generated.h"

UCLASS()
class ADestructableResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer);
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType);

	virtual void BeginPlay() override;

	ECraftResourceType GetResourceType() const { return ResourceType; }

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	UInteractableComponent* InteractableComponent;

protected:
	ECraftResourceType ResourceType{ ECraftResourceType::Invalid };

	UPROPERTY(EditAnywhere)
	int ResourceDropCount;

	UPROPERTY(EditAnywhere)
	float Health;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
};
