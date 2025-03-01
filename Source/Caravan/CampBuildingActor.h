#pragma once

#include "CampBuildingActor.generated.h"

class UCampBuildingSpec;
class UInteractableComponent;
class UInventoryComponent;

UENUM(BlueprintType)
enum class ECampBuildingState : uint8
{
	Deconstructed,
	Constructed,
	// Open/Close?
	Invalid
};

UCLASS()
class ACampBuildingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACampBuildingActor();

	virtual void BeginPlay() override;

	void SetActive(bool bValue);

	UFUNCTION(BlueprintCallable)
	void ConstructBuilding(float PercentAmount);

	UFUNCTION(BlueprintCallable)
	void DeconstructBuilding();

	UFUNCTION(BlueprintCallable)
	bool IsConstructed() const { return ConstructedPercent == 1.f; }

	UFUNCTION(BlueprintCallable)
	bool CraftItem(APawn* InteractingPawn, int InventoryItemIndex);

protected:
	ECampBuildingState GetBuildingState() const { return BuildingState; }
	void SetBuildingState(ECampBuildingState InState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingStateChange(ECampBuildingState OldState, ECampBuildingState NewState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingConstructProgress(float CurrentPercent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingConstructed();

	// TODO : Should this be set from caravan construction rather than default value?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	UCampBuildingSpec* CampBuildingSpec;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractableComponent* InteractableComponent;
	
	// Items that can be crafted are stored in the inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	float ConstructedPercent = 0.f;

	// Currently not saved as it is set on BeginPlay based on Constructed state
	UPROPERTY(Transient, BlueprintReadOnly);
	ECampBuildingState BuildingState = ECampBuildingState::Invalid;

	// Active when the Caravan is open and the building is available to interact with
	bool bActive = false;
};
