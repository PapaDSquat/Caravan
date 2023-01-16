#pragma once

#include "PickupItemActor.generated.h"

class UInteractableComponent;
struct FInteractionChoice;
struct FInventoryItemDataRow;

UCLASS()
class CARAVAN_API APickupItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupItemActor(const class FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;

	void InitializeItem(const FDataTableRowHandle& InItemHandle, const FInventoryItemDataRow& InItemDef);

	ECraftResourceType GetResourceType() const;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UInteractableComponent* InteractableComponent;

	UPROPERTY(BlueprintReadOnly)
	FDataTableRowHandle ItemHandle;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
};
