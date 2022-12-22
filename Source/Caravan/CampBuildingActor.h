#pragma once

#include "CampBuildingActor.generated.h"

class UInteractableComponent;

UENUM(BlueprintType)
enum class ECampBuildingState : uint8
{
	Unconstructed,
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

	UFUNCTION(BlueprintCallable)
	void ConstructBuilding(float PercentAmount);

	UFUNCTION(BlueprintCallable)
	bool IsConstructed() const { return ConstructedPercent == 1.f; }

protected:
	ECampBuildingState GetBuildingState() const { return BuildingState; }
	void SetBuildingState(ECampBuildingState InState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingStateChange(ECampBuildingState OldState, ECampBuildingState NewState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingConstructProgress(float CurrentPercent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingConstructed();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractableComponent* InteractableComponent;
	
	UPROPERTY(SaveGame, BlueprintReadOnly)
	float ConstructedPercent = 0.f;

	// Currently not saved as it is set on BeginPlay based on Constructed state
	UPROPERTY(Transient, BlueprintReadOnly);
	ECampBuildingState BuildingState = ECampBuildingState::Invalid;
};
