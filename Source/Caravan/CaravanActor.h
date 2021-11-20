// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InteractableComponent.h"
#include "CaravanActor.generated.h"

class UStaticMeshSocket;
class ACaravanBuildingPlatform;

UENUM()
enum ECaravanBuildingType
{
	Invalid,
	CraftStation
};

// TODO : Rename
UCLASS()
class ACaravanActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACaravanActor(const class FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<class ACaravanBuildingPlatform> BuildingPlatformBPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Settings")
	int BuildingGridTotalRows= 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Settings")
	int BuildingGridTotalColumns = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan | State")
	bool bOpenOnBegin = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Caravan | State")
	bool IsOpen = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Caravan | State")
	void NotifyOnToggleOpen(bool Open);

	const UStaticMeshSocket* GetCarrySocket() const;
	FVector GetCarrySocketLocation() const;
	FRotator GetCarrySocketRotation() const;

	// IInteractable
	//virtual FString GetInteractionName() const override;
	//virtual void OnInteractFocus(const InteractData& interactData) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractableComponent* InteractableFrontComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractableComponent* InteractableBackComponent;

private:
	void SetCaravanOpen(bool bOpen);

	ACaravanBuildingPlatform* CreateBuildingAttachment(ECaravanBuildingType buildingType, const FIntPoint& gridPosition);
	ACaravanBuildingPlatform* GetBuildingAttachment(const FIntPoint& gridPosition) const;
	bool SetBuildingAttachment(const FIntPoint& gridPosition, ACaravanBuildingPlatform* actor);

	UFUNCTION()
	void OnInteractWithFront(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
	UFUNCTION()
	void OnInteractWithBack(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);

	// TODO: Potentially move to Interactable? (needs base StaticMeshComponent or getter)
	struct SocketInteractionData
	{
		SocketInteractionData(EInteractionType interactionType, FName socketName)
			: InteractionType(interactionType), SocketName(socketName) {}
		EInteractionType InteractionType;
		FName SocketName;
	};

	TArray< TArray<ACaravanBuildingPlatform*> > BuildingAttachmentGrid;

	EInteractionType FocusedInteractionType{ EInteractionType::None };
};
