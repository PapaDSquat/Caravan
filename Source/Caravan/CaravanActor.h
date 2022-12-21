#pragma once

#include "Components/InteractableComponent.h"

#include "CaravanActor.generated.h"

class ACaravanBuildingPlatform;
class ARobotAICharacter;
class UAIRobotCharacterSpec;
class UArrowComponent;
class UStaticMeshSocket;

UENUM()
enum ECaravanBuildingType
{
	Invalid,
	CraftStation
};

// Temporary container for choosing default robots in the Caravan
USTRUCT(BlueprintType)
struct FCaravanInitialRobotData
{
	GENERATED_BODY()

	bool ShouldSpawn() const { return bEnabled && RobotSpec != nullptr; }

	// If false, will not be spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAIRobotCharacterSpec* RobotSpec = nullptr;
};

// TODO : Rename
UCLASS()
class ACaravanActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACaravanActor(const class FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Caravan")
	const FVector& GetCampAreaCenter() const;

	UFUNCTION(BlueprintCallable, Category = "Caravan")
	float GetCampAreaRadius() const;

	UFUNCTION(BlueprintCallable, Category = "Caravan")
	bool IsCampAreaObstructed() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<class ACaravanBuildingPlatform> BuildingPlatformBPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan|Building Settings")
	int BuildingGridTotalRows= 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan|Building Settings")
	int BuildingGridTotalColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan|Building Settings")
	float BuildingGridCellSize = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan|State")
	bool bOpenOnBegin = false;

	UPROPERTY(BlueprintReadOnly, Category = "Caravan|State")
	bool IsOpen = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Caravan|State")
	void NotifyOnToggleOpen(bool Open);

	const UStaticMeshSocket* GetCarrySocket() const;
	FVector GetCarrySocketLocation() const;
	FRotator GetCarrySocketRotation() const;

	// IInteractable
	//virtual FString GetInteractionName() const override;
	//virtual void OnInteractFocus(const InteractData& interactData) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan|AI")
	TArray< FCaravanInitialRobotData > InitialRobots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* BuildDirectionComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UInteractableComponent* InteractableFrontComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UInteractableComponent* InteractableBackComponent = nullptr;

private:
	void GenerateCampArea();
	void SetCaravanOpen(bool bOpen, bool bAlwaysFireEvent = false);

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
	FVector CampAreaCenterLocation;
	float CampAreaRadius = 0.f;

	TArray< ARobotAICharacter* > Robots;

	EInteractionType FocusedInteractionType{ EInteractionType::None };
};
