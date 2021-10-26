// Fill out your copyright notice in the Description page of Project Settings.

#include "CaravanActor.h"
#include "Caravan.h"
#include "CaravanCharacter.h"
#include "CaravanBuildingPlatform.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

static const FName SOCKET_PULL("PullSocket");
static const FName SOCKET_TOGGLE_OPEN("ToggleOpenSocket");
static const FName SOCKET_BUILDING_ATTACHMENT("BuildingAttachmentSocket");

// Sets default values
ACaravanActor::ACaravanActor(const class FObjectInitializer& ObjInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//// TODO : Get rid of this shit
	//static ConstructorHelpers::FClassFinder<ACaravanBuildingPlatform> buildingPlatformBP(TEXT("/Caravan/Content/ThirdPerson/Player/Caravan/BP_CaravanBuildingPlatform"));
	//if (buildingPlatformBP.Class != NULL)
	//{
	//	BuildingPlatformBPClass = buildingPlatformBP.Class;
	//}

	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CaravanActor_StaticMeshComponent"));
}

// Called when the game starts or when spawned
void ACaravanActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize the grid to its fixed size
	BuildingAttachmentGrid.SetNum(BuildingGridTotalRows);
	for (TArray<ACaravanBuildingPlatform*>& row : BuildingAttachmentGrid)
	{
		row.SetNum(BuildingGridTotalColumns);
	}

	
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(1, 0));
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(0, 1));
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(1, 1));
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(2, 1));
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(0, 2));
	CreateBuildingAttachment(ECaravanBuildingType::CraftStation, FIntPoint(2, 2));
	
}

ACaravanBuildingPlatform* ACaravanActor::CreateBuildingAttachment(ECaravanBuildingType buildingType, const FIntPoint& gridPosition)
{
	// TODO: Use GridGenerator sub-cells to generate positions
	// TODO if grid position is available

	FActorSpawnParameters SpawnParameters;
	ACaravanBuildingPlatform* buildingPlatformActor = GetWorld()->SpawnActor<ACaravanBuildingPlatform>(BuildingPlatformBPClass, SpawnParameters);
	if (buildingPlatformActor)
	{
		// TODO: Attach?
		SetBuildingAttachment(gridPosition, buildingPlatformActor);
	}
	return buildingPlatformActor;
}

void ACaravanActor::SetCaravanOpen(bool bOpen)
{
	if (IsOpen != bOpen)
	{
		IsOpen = bOpen;

		NotifyOnToggleOpen(IsOpen);

		return;
		int rows = BuildingAttachmentGrid.Num();
		for (int gridX = 0; gridX < rows; ++gridX)
		{
			TArray<ACaravanBuildingPlatform*>& rowArray = BuildingAttachmentGrid[gridX];
			int columns = rowArray.Num();
			for (int gridY = 0; gridY < columns; ++gridY)
			{
				FIntPoint gridPosition(gridX, gridY);
				if (ACaravanBuildingPlatform* buildingPlatformActor = rowArray[gridY])
				{
					buildingPlatformActor->SetActive(bOpen);

					// Position platforms in grid
					// Grid direction is along the normal of the back face of the Caravan.
					// The Cell Count Y is for rows, and X for columns.
					if (bOpen && StaticMeshComponent)
					{
						// Start position at the center of rows and 0 position of columns
						const FVector caravanBackward = GetActorForwardVector() * -1.f;
						const FVector caravanLeft = GetActorRightVector() * -1.f;
						
						const FVector socketLocation = StaticMeshComponent->GetSocketLocation(SOCKET_BUILDING_ATTACHMENT);

						FVector gridCellOrigin, gridCellSize;
						buildingPlatformActor->GetActorBounds(true, gridCellOrigin, gridCellSize);
						gridCellSize *= 2.f;

						const FVector gridTotalSize = gridCellSize * FVector((float)BuildingGridTotalColumns, (float)BuildingGridTotalRows, 1.f);

						// Offset by half rows so it is centered
						int rowOffset = BuildingGridTotalRows / 2;
						FVector positionOffset = FVector((float)gridPosition.Y, (float)(gridPosition.X- rowOffset), 0.f) * gridCellSize;

						FVector buildingPosition = socketLocation;
						buildingPosition += caravanBackward * positionOffset.X;
						buildingPosition += caravanLeft * positionOffset.Y;

						buildingPlatformActor->SetActorRotation(GetActorRotation());
						buildingPlatformActor->SetActorLocation(buildingPosition);

						// DEBUG
						/*
						DrawDebugBox(
							GetWorld(),
							gridCellOrigin,
							gridCellSize,
							FColor(255, 0, 255), // Pink
							false, 30.1f
							);

						DrawDebugSphere(
							GetWorld(),
							socketLocation,
							75.f,
							16,
							FColor(255, 0, 255),// Pink
							false, 30.1
							);
						DrawDebugLine(
							GetWorld(),
							socketLocation,
							socketLocation + caravanBackward * 500.f,
							FColor(0, 0, 255),// Blue
							false, 30.1, 0,
							10.f
							);

						DrawDebugLine(
							GetWorld(),
							socketLocation,
							socketLocation + caravanLeft * 500.f,
							FColor(255, 0, 255),// pink
							false, 30.1, 0,
							10.f
							);
						*/
					}
				}
			}
		}
	}
}

ACaravanBuildingPlatform* ACaravanActor::GetBuildingAttachment(const FIntPoint& gridPosition) const
{
	ACaravanBuildingPlatform* foundBuildingPlatformActor = NULL;

	// ASSERT
	if (gridPosition.X < BuildingAttachmentGrid.Num())
	{
		const TArray<ACaravanBuildingPlatform*>& column = BuildingAttachmentGrid[gridPosition.X];
		if (gridPosition.Y < column.Num())
		{
			return column[gridPosition.Y];
		}
	}

	return foundBuildingPlatformActor;
}

bool ACaravanActor::SetBuildingAttachment(const FIntPoint& gridPosition, ACaravanBuildingPlatform* actor)
{
	if (gridPosition.X < BuildingAttachmentGrid.Num())
	{
		TArray<ACaravanBuildingPlatform*>& column = BuildingAttachmentGrid[gridPosition.X];
		if (gridPosition.Y < column.Num())
		{
			column[gridPosition.Y] = actor;
			return true;
		}
	}
	return false;
}

// Called every frame
void ACaravanActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

FString ACaravanActor::GetInteractionName() const
{
	FString interactionName("");
	switch (FocusedInteractionType)
	{
	case EInteractionType::CaravanPull: interactionName = "PULL CARAVAN"; break;
	case EInteractionType::CaravanToggleOpen: interactionName = (IsOpen ? "CLOSE CARAVAN" : "OPEN CARAVAN"); break;
	case EInteractionType::CaravanBuildAttachment: interactionName = "BUILD ATTACHMENT"; break;
	}
	return interactionName;
}

void ACaravanActor::OnInteractFocus(const InteractData& interactData)
{
	AInteractableActor::OnInteractFocus(interactData);

	if (StaticMeshComponent)
	{
		const FVector& impactPoint = interactData.TraceResult.ImpactPoint;

		TArray<SocketInteractionData> socketInteractionList;
		socketInteractionList.Push(SocketInteractionData(EInteractionType::CaravanToggleOpen, SOCKET_TOGGLE_OPEN));
		if (!IsOpen)
		{
			// Only allow pull if the caravan is closed
			socketInteractionList.Push(SocketInteractionData(EInteractionType::CaravanPull, SOCKET_PULL));
		}

		int closestSocketIndex = 0;
		float closestSocketDistance = 9999999.f;
		for (int i = 0; i < socketInteractionList.Num(); ++i)
		{
			const SocketInteractionData& socketInteractionData = socketInteractionList[i];
			FVector socketLocation = StaticMeshComponent->GetSocketLocation(socketInteractionData.SocketName);
			float socketDistance = (socketLocation - impactPoint).Size();
			if (socketDistance < closestSocketDistance)
			{
				closestSocketDistance = socketDistance;
				closestSocketIndex = i;
			}
		}

		const SocketInteractionData& selectedInteractionData = socketInteractionList[closestSocketIndex];
		FocusedInteractionType = selectedInteractionData.InteractionType;
	}
	else
	{
		FocusedInteractionType = EInteractionType::None;
	}
}

EInteractionType ACaravanActor::OnInteractSelect(const InteractData& interactData)
{
	AInteractableActor::OnInteractSelect(interactData);

	switch (FocusedInteractionType)
	{
	case EInteractionType::CaravanToggleOpen:
		SetCaravanOpen(!IsOpen);
		break;
	}
	return FocusedInteractionType;
}

// TODO: Remove these
const UStaticMeshSocket* ACaravanActor::GetCarrySocket() const
{
	return StaticMeshComponent->GetSocketByName(SOCKET_PULL);
}

FVector ACaravanActor::GetCarrySocketLocation() const
{
	return StaticMeshComponent->GetSocketLocation(SOCKET_PULL);
}

FRotator ACaravanActor::GetCarrySocketRotation() const
{
	return StaticMeshComponent->GetSocketRotation(SOCKET_PULL);
}