// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CraftResourceActor.h"
#include "DestructableResourceActor.generated.h"

class UInteractableComponent;

UCLASS()
class ADestructableResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer);
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType);

	virtual void BeginPlay() override;

	ECraftResourceType GetResourceType() const { return ResourceType; }

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, Category="Resource")
	ECraftResourceType ResourceType{ ECraftResourceType::Invalid };

protected:

	UPROPERTY(EditAnywhere)
	int ResourceDropCount;

	UPROPERTY(EditAnywhere)
	float Health;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);

	template< class TActorClass >
	TActorClass* SpawnResourceActor(const TSubclassOf<TActorClass>& ActorClass, int ActorIndex);
};
