// Fill out your copyright notice in the Description page of Project Settings.

#include "Caravan.h"
#include "CaravanActor.h"
#include "CaravanCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "RockActor.h"
#include "TreeActor.h"
#include "WorldBuilder/WorldBuilderSubsystem.h"
#include "WorldBuilder/WorldGenerationSpec.h"

static const float CELL_ACTOR_SPAWN_RAYCAST_LENGTH = 5000.f;
static const float CELL_ACTOR_SPAWN_OFFSET_Y = 30.f; // To ensure the bottom edge isn't visible
static const float DEBUG_TIME_REVERSE_FORWARD_SPEED = 50.f; // 1.0 = default speed

void UWorldBuilderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UWorldBuilderSubsystem::Deinitialize()
{

}

void UWorldBuilderSubsystem::UpdateTime(float DeltaTime)
{
	if (DEBUG_TimeReverse)
	{
		CurrentTimeSpeed = -DEBUG_TIME_REVERSE_FORWARD_SPEED;
	}
	else if (DEBUG_TimeForward)
	{
		CurrentTimeSpeed = DEBUG_TIME_REVERSE_FORWARD_SPEED;
	}
	else
	{
		CurrentTimeSpeed = 1.0f;
	}

	float oldElapsedTime = CurrentDayElapsedTimeSeconds;
	CurrentDayElapsedTimeSeconds += DeltaTime * CurrentTimeSpeed;

	// The 'Set Sun Pitch' BP node takes an amount to move the pitch in a range between -90 and 90
	// NOTE: This has to happen before we adjust for min/max
	CurrentSunPitch = (180.f * (CurrentDayElapsedTimeSeconds - oldElapsedTime) / SecondsPerDay);

	// Handle reversing time
	if (CurrentDayElapsedTimeSeconds <= 0)
	{
		// Previous day
		--CurrentDay;

		CurrentDayElapsedTimeSeconds = SecondsPerDay + CurrentDayElapsedTimeSeconds;
	}
	// Forward time
	else
	{
		float dayOverlap = CurrentDayElapsedTimeSeconds - SecondsPerDay;
		if (dayOverlap > 0.f)
		{
			// Next day
			++CurrentDay;

			CurrentDayElapsedTimeSeconds = dayOverlap;
		}
	}
}

bool UWorldBuilderSubsystem::GenerateNewWorld(const FVector& Position, UWorldGenerationSpec const * Spec, const struct FWorldGenerationActors& Actors, const struct FWorldGenerationTimeParams& TimeParams)
{
	if (!ensureMsgf(Spec != NULL, TEXT("[UWorldBuilderSubsystem::GenerateNewWorld] World Generation Spec is invalid")))
	{
		return false;
	}

	{
		GeneratePosition = Position;
		WorldSpec = Spec;
		PlayerCharacter = Actors.PlayerCharacter;
		CaravanActor = Actors.CaravanActor;
		SecondsPerDay = TimeParams.SecondsPerDay;
		TimeDisplayMinuteInterval = TimeParams.TimeDisplayMinuteInterval;
	}

	// Initialize the grid to its fixed size
	GridData.Empty();
	GridData.SetNum(WorldSpec->GridCellCountX);
	for (TArray<GridCellData>& row : GridData)
	{
		row.SetNum(WorldSpec->GridCellCountY);
	}

	ResetResourceGrid();

	// Set initial time
	// TODO: Offset sun from InitialTimeOfDayRatio
	CurrentDayElapsedTimeSeconds = TimeParams.SecondsPerDay * TimeParams.InitialTimeOfDayRatio;

	return true;
}

void UWorldBuilderSubsystem::ResetResourceGrid()
{
	TArray<GridCellReserveSlot> reservedCells;

	// TODO: Generation avoids player and caravan
	//if (PlayerCharacter && PlayerCell.X >= 0 && PlayerCell.Y >= 0)
	//{
	//	reservedCells.Push(GridCellReserveSlot(PlayerCell, PlayerCharacter));
	//}
	//
	//if (CaravanCell.X >= 0 && CaravanCell.Y >= 0)
	//{
	//	reservedCells.Push(GridCellReserveSlot(CaravanCell, CaravanActor));
	//}

	// For now, some magic to determine relative amounts of each resource
	// Re-calculate the density so the total combined densities falls between 0 and 1
	float relativeFactor = 1.0f / (WorldSpec->TreeDensity + WorldSpec->RockDensity);
	float realTreeDensity = WorldSpec->TreeDensity * relativeFactor;
	float realRockDensity = WorldSpec->RockDensity * relativeFactor;

	// Spawn actors in grid
	int rows = GridData.Num();
	for (int gridX = 0; gridX < rows; ++gridX)
	{
		TArray<GridCellData>& rowArray = GridData[gridX];
		int columns = rowArray.Num();
		for (int gridY = 0; gridY < columns; ++gridY)
		{
			GridCellData& cellData = rowArray[gridY];
			FIntPoint gridPosition(gridX, gridY);

			if (WorldSpec->SpawnGameActorsEnabled)
			{
				bool bCellIsReserved = false;
				for (const GridCellReserveSlot& reservedCell : reservedCells)
				{
					if (reservedCell.CellPosition == gridPosition)
					{
						// Reserved cell, so position the actor, then remove from array
						if (reservedCell.Actor != NULL)
						{
							FVector cellTopLeft, cellBottomRight;
							GetGridCellBounds(reservedCell.CellPosition, cellTopLeft, cellBottomRight);

							// First position actor at center of the cell, then perform raycast downwards
							// to determine its position along the terrain
							FVector traceStartLocation = cellTopLeft + ((cellBottomRight - cellTopLeft) / 2.f);
							reservedCell.Actor->SetActorLocation(traceStartLocation);

							FHitResult hitResult;
							if (PerformTerrainRaycast(reservedCell.Actor, hitResult))
							{
								FVector actorOrigin, actorBB;
								reservedCell.Actor->GetActorBounds(true, actorOrigin, actorBB);

								// Offset from impact point by the bounding box of the actor
								FVector finalPosition = hitResult.ImpactPoint + traceStartLocation.Z - actorOrigin.Z + actorBB.Z;

								reservedCell.Actor->SetActorLocation(finalPosition);
							}
						}
						bCellIsReserved = true;
						break;
					}
				}
				if (bCellIsReserved) // Reserved cell so jump to the next one
					continue;
			}

			if (WorldSpec->SpawnResourceActorsEnabled)
			{
				// If it already contains an actor, destroy it.
				// This is critical when resetting the grid
				if (cellData.Actor != NULL)
				{
					cellData.Actor->Destroy();
				}

				// Random chance to spawn based on total actor density
				if (FMath::RandRange(0.f, 1.0f) <= WorldSpec->TotalCraftActorDensity)
				{
					// Determine if it's a tree or rock by seeing if a random number
					// falls within the density range of either one
					float randResourceIndex = FMath::RandRange(0.f, 1.0f);
					if (randResourceIndex <= realTreeDensity)
					{
						SpawnGridCellActor<ATreeActor>(
							cellData,
							FName(TEXT("/Game/Caravan/Meshes/Tree1")),
							gridPosition
							);
					}
					else
					{
						SpawnGridCellActor<ARockActor>(
							cellData,
							FName(TEXT("/Game/Caravan/Meshes/Rock1")),
							gridPosition
							);
					}
				}
			}
		}
	}
}

template< class ActorClass >
ActorClass* UWorldBuilderSubsystem::SpawnGridCellActor(GridCellData& cellData, const FName& meshName, const FIntPoint& gridPosition)
{
	FVector gridOrigin = GeneratePosition;

	FActorSpawnParameters SpawnParameters;
	if (ActorClass* spawnedActor = GetWorld()->SpawnActor<ActorClass>(SpawnParameters))
	{
		// Random scale
		float scaleAmount = FMath::RandRange(WorldSpec->CraftActorScaleMin, WorldSpec->CraftActorScaleMax);
		FVector scale = FVector(scaleAmount, scaleAmount, scaleAmount);
		spawnedActor->SetActorScale3D(scale);

		FBox actorBB;
		if (spawnedActor->StaticMeshComponent)
		{
			if (UStaticMesh* ResourceMesh = DynamicLoadObjFromPath<UStaticMesh>(meshName))
			{
				actorBB = ResourceMesh->GetBoundingBox();
				//actorBB.ExpandBy(scale);
				spawnedActor->StaticMeshComponent->SetStaticMesh(ResourceMesh);
			}
		}

		FVector cellTopLeft, cellBottomRight;
		GetGridCellBounds(gridPosition, cellTopLeft, cellBottomRight);

		// Position in random position within the cell
		float randomX = FMath::RandRange(cellTopLeft.X, cellBottomRight.X);
		float randomY = FMath::RandRange(cellTopLeft.Y, cellBottomRight.Y);

		FVector finalPosition(randomX, randomY, gridOrigin.Z);

		// We've selected random X and Y, now raycast downwards to find the Z on the terrain
		FHitResult hitResult(ForceInit);
		if (PerformTerrainRaycast(finalPosition, CELL_ACTOR_SPAWN_RAYCAST_LENGTH, hitResult))
		{
			// Offset the actor by its bounding box so it's bottom rests on the terrain
			float offsetFromCenter = spawnedActor->GetActorLocation().Z - actorBB.GetCenter().Z;
			float halfHeight = actorBB.GetExtent().Z;
			float totalOffset = (offsetFromCenter + halfHeight - CELL_ACTOR_SPAWN_OFFSET_Y) * scaleAmount;
			if (hitResult.IsValidBlockingHit())
			{
				finalPosition.Z = hitResult.ImpactPoint.Z + totalOffset;
			}
		}

		spawnedActor->SetActorLocation(finalPosition);

		// Random yaw orientation
		FRotator rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
		spawnedActor->SetActorRotation(rotation);

		// Update the cell data
		cellData.Actor = spawnedActor;

		return spawnedActor;
	}
	return NULL;
}

bool UWorldBuilderSubsystem::PerformTerrainRaycast(const AActor* Actor, FHitResult& hitResult)
{
	if (Actor)
	{
		FVector traceStart = Actor->GetActorLocation();
		return PerformTerrainRaycast(traceStart, CELL_ACTOR_SPAWN_RAYCAST_LENGTH, hitResult);
	}
	return false;
}

bool UWorldBuilderSubsystem::PerformTerrainRaycast(const FVector& traceStart, float length, FHitResult& hitResult)
{
	FVector traceEnd = traceStart;
	traceEnd.Z -= length;

	FName traceName = FName(TEXT("WorldGeneratorCellTrace"));
	FCollisionQueryParams TraceParams = FCollisionQueryParams(traceName, true);
	FCollisionObjectQueryParams TraceObjectParams = FCollisionObjectQueryParams::AllStaticObjects;

	// TODO: Handle when already colliding
	bool bHit = GetWorld()->LineTraceSingleByObjectType(hitResult, traceStart, traceEnd, TraceObjectParams, TraceParams);
	return bHit;
}

void UWorldBuilderSubsystem::GetGridCellBounds(const FIntPoint& cellPosition, FVector& topLeft, FVector& bottomRight) const
{
	topLeft = GeneratePosition;
	topLeft.X += cellPosition.X * WorldSpec->GridCellSizeX;
	topLeft.Y += cellPosition.Y * WorldSpec->GridCellSizeY;

	bottomRight = topLeft;
	bottomRight.X += WorldSpec->GridCellSizeX;
	bottomRight.Y += WorldSpec->GridCellSizeY;
}

FString UWorldBuilderSubsystem::GetCurrentTimeString() const
{
	FString timeString;

	static const float HOURS_PER_DAY = 24.f;
	static const float MINUTES_PER_HOUR = 60.f;
	static const float SECONDS_PER_MINUTE = 60.f;
	static const float SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
	static const float SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;

	// SecondsPerDay is equivalent to 24 hours
	const float REAL_TIME_RATIO = SECONDS_PER_DAY / SecondsPerDay;

	// Convert everything into real time and divide to get hours and minutes
	float realElapsedSeconds = CurrentDayElapsedTimeSeconds * REAL_TIME_RATIO;
	float remainingSeconds = realElapsedSeconds;

	int realHour = (int)(remainingSeconds / SECONDS_PER_HOUR);
	remainingSeconds -= (float)realHour * SECONDS_PER_HOUR;

	int realMinutes = (int)(remainingSeconds / SECONDS_PER_MINUTE);

	// Display only on fixed intervals
	int remainder = (realMinutes % TimeDisplayMinuteInterval);
	realMinutes -= remainder;

	// Convert to AM/PM
	// realHour ranges from 0 to 23, to convert to 1 to 12
	int realHourAMPM = (realHour % 12) + 1;

	timeString += FString::FromInt(realHourAMPM);
	timeString += ".";
	if (realMinutes < 10)
		timeString += "0";
	timeString += FString::FromInt(realMinutes);

	timeString += (realHour <= 12) ? ".AM" : ".PM";

	return timeString;
}