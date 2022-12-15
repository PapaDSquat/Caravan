#include "CaravanActor.h"

#include "AI/AIRobotSubsystem.h"
#include "Caravan.h"
#include "CaravanCharacter.h"
#include "CaravanBuildingPlatform.h"
#include "Components/ArrowComponent.h"
#include "Components/InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "Utils/CaravanEngineUtils.h"

static const FName SOCKET_PULL("PullSocket");
static const FName SOCKET_TOGGLE_OPEN("ToggleOpenSocket");
static const FName SOCKET_BUILDING_ATTACHMENT("BuildingAttachmentSocket");

ACaravanActor::ACaravanActor(const class FObjectInitializer& ObjInitializer)
{
	USkeletalMeshComponent* SkeletalMesh = ObjInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));;
	SetRootComponent(SkeletalMesh);

	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CaravanActor_StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	BuildDirectionComponent = ObjInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("BuildDirection"));
	BuildDirectionComponent->SetupAttachment(RootComponent);

	InteractableFrontComponent = ObjInitializer.CreateDefaultSubobject<UInteractableComponent>(this, TEXT("InteractableFrontComponent"));
	InteractableFrontComponent->SetupAttachment(RootComponent);

	InteractableBackComponent = ObjInitializer.CreateDefaultSubobject<UInteractableComponent>(this, TEXT("InteractableBackComponent"));
	InteractableBackComponent->SetupAttachment(RootComponent);
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
	
	// Event Registration
	if (IsValid(InteractableFrontComponent))
	{
		InteractableFrontComponent->OnInteract.AddDynamic(this, &ACaravanActor::OnInteractWithFront);
	}
	if (IsValid(InteractableBackComponent))
	{
		InteractableBackComponent->OnInteract.AddDynamic(this, &ACaravanActor::OnInteractWithBack);
	}

	SetCaravanOpen(bOpenOnBegin, true);
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

void ACaravanActor::SetCaravanOpen(bool bOpen, bool bAlwaysFireEvent /*= false*/)
{
	const bool bChanged = IsOpen != bOpen;
	if (bChanged)
	{
		IsOpen = bOpen;

		// TODO: Move this out of here
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UAIRobotSubsystem* AIRobotSubsystem = GameInstance->GetSubsystem<UAIRobotSubsystem>();
		if (AIRobotSubsystem && InitialRobots.Num() > 0)
		{
			if (IsOpen)
			{
				int TotalRobotsToSpawn = 0;
				for (const FCaravanInitialRobotData& RobotData : InitialRobots)
				{
					if (RobotData.ShouldSpawn())
						++TotalRobotsToSpawn;
				}

				static float s_SpawnAngleRange = 90.f;
				static float s_SpawnDistance = 600.f;
				const float AngleIncrement = s_SpawnAngleRange / (TotalRobotsToSpawn - 1);

				const FVector CaravanLocation = GetActorLocation();
				const FVector Forward = GetActorForwardVector() * FVector(-1.f, -1.f, 1.f);

				float CurrentAngle = TotalRobotsToSpawn == 1 ? 0.f : (s_SpawnAngleRange * -0.5f);

				for (const FCaravanInitialRobotData& RobotData : InitialRobots)
				{
					if (!RobotData.bEnabled)
						continue;

					UAIRobotCharacterSpec* RobotSpec = RobotData.RobotSpec;
					if (!RobotSpec)
						continue;

					const FVector SpawnDirection = Forward.RotateAngleAxis(CurrentAngle, FVector::UpVector);
					const FVector SpawnLocation = CaravanLocation + (SpawnDirection * s_SpawnDistance);
					const FTransform SpawnTransform(SpawnDirection.ToOrientationRotator(), SpawnLocation);
					if (ARobotAICharacter* SpawnedRobot = AIRobotSubsystem->SpawnRobotCharacter(RobotSpec, SpawnTransform))
					{
						Robots.Add(SpawnedRobot);
					}

					CurrentAngle += AngleIncrement;
				}
			}
			else
			{
				for (ARobotAICharacter* Robot : Robots)
				{
					AIRobotSubsystem->DespawnRobotCharacter(Robot);
				}
				Robots.Empty();
			}
		}
	}

	if (bChanged || bAlwaysFireEvent)
	{
		NotifyOnToggleOpen(IsOpen);
	}


	// Start position at the center of rows and 0 position of columns
	const FVector caravanBackward = GetActorForwardVector() * -1.f;
	const FVector caravanLeft = GetActorRightVector() * -1.f;

	const FVector socketLocation = BuildDirectionComponent->GetComponentLocation();

	// Position platforms in grid
	// Grid direction is along the normal of the back face of the Caravan.
	// The Cell Count Y is for rows, and X for columns.
	const FVector gridTotalSize = BuildingGridCellSize * FVector((float)BuildingGridTotalColumns, (float)BuildingGridTotalRows, 1.f);

	const FVector gridOffset(0.f, gridTotalSize.X * 0.5f, 0.f);

	int rows = BuildingAttachmentGrid.Num();
	for (int gridX = 0; gridX < rows; ++gridX)
	{
		TArray<ACaravanBuildingPlatform*>& rowArray = BuildingAttachmentGrid[gridX];
		int columns = rowArray.Num();
		for (int gridY = 0; gridY < columns; ++gridY)
		{
			FIntPoint gridPosition(gridX, gridY);
			
			// Offset by half rows so it is centered
			FVector positionOffset = (FVector((float)gridPosition.Y, (float)gridPosition.X, 0.f) * BuildingGridCellSize) - gridOffset;

			FVector buildingPosition = socketLocation;
			buildingPosition += caravanBackward * positionOffset.X;
			buildingPosition += caravanLeft * positionOffset.Y;

			if (ACaravanBuildingPlatform* buildingPlatformActor = rowArray[gridY])
			{
				buildingPlatformActor->SetActive(bOpen);

				buildingPlatformActor->SetActorRotation(GetActorRotation());
				buildingPlatformActor->SetActorLocation(buildingPosition);
			}

			DrawDebugSphere(
				GetWorld(),
				buildingPosition,
				32.f,
				16,
				FColor::White,
				false, 
				30.f
			);
		}
	}

	// DEBUG
	DrawDebugSphere(
		GetWorld(),
		socketLocation,
		64.f,
		16,
		FColor::Green,
		false, 
		30.f
	);
	DrawDebugLine(
		GetWorld(),
		socketLocation,
		socketLocation + caravanBackward * 500.f,
		FColor::Blue,
		false, 
		30.f, 
		0,
		10.f
	);

	DrawDebugLine(
		GetWorld(),
		socketLocation,
		socketLocation + caravanLeft * 500.f,
		FColor::Red,
		false, 30.f, 
		0,
		10.f
	);
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

/*
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
	Super::OnInteractFocus(interactData);

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
*/

void ACaravanActor::OnInteractWithFront(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	// TODO
}

void ACaravanActor::OnInteractWithBack(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	SetCaravanOpen(!IsOpen);
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