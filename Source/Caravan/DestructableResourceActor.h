#pragma once

#include "GameFramework/Actor.h"
#include "CraftResourceActor.h"

#include "DestructableResourceActor.generated.h"

class APickupItemActor;
class UInteractableComponent;
struct FDataTableRowHandle;

UCLASS()
class ADestructableResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer);
	ADestructableResourceActor(const class FObjectInitializer& ObjInitializer, ECraftResourceType resourceType);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "RPG")
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

	APickupItemActor* SpawnResourceActor(const FDataTableRowHandle& ItemHandle, int ActorIndex);
};
