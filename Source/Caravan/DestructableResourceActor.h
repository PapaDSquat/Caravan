// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "CraftResourceActor.h"
#include "DestructableResourceActor.generated.h"

UCLASS()
class ADestructableResourceActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer);
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType);

	virtual void BeginPlay() override;

	// IInteractable
	virtual ECraftResourceType GetResourceType() const override { return ResourceType; }
	//virtual void OnInteractFocus(const InteractData& interactData) override;
	//virtual EInteractionType OnInteractSelect(const InteractData& interactData) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

protected:
	ECraftResourceType ResourceType{ ECraftResourceType::Invalid };

	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere)
	int ResourceDropCount;

	UPROPERTY(EditAnywhere)
	float Health;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
};
