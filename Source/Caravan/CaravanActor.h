// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InteractableComponent.h"
#include "CaravanActor.generated.h"

class ACaravanBuildingPlatform;
class UAIRobotCharacterSpec;
class ARobotAICharacter;
class UStaticMeshSocket;

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
	int BuildingGridTotalRows= 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Settings")
	int BuildingGridTotalColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan | State")
	bool bOpenOnBegin = false;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray< UAIRobotCharacterSpec* > DefaultRobotSpecs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Interaction")
	UInteractableComponent* InteractableFrontComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Interaction")
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

	TArray< ARobotAICharacter* > Robots;

	EInteractionType FocusedInteractionType{ EInteractionType::None };
};
