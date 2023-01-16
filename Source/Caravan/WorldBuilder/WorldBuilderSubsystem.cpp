#include "WorldBuilder/WorldBuilderSubsystem.h"

#include "AI/EnemyAIController.h"
#include "Caravan.h"
#include "CaravanActor.h"
#include "CaravanCharacter.h"
#include "DestructableResourceActor.h"
#include "Math/UnrealMathUtility.h"
#include "RockActor.h"
#include "RPG/PickupItemActor.h"
#include "TreeActor.h"
#include "Utils/CaravanCollisionTypes.h"
#include "WorldBuilder/WorldGenerationSpec.h"

static const float CELL_ACTOR_SPAWN_RAYCAST_LENGTH = 5000.f;
static const float CELL_ACTOR_SPAWN_OFFSET_Y = 30.f; // To ensure the bottom edge isn't visible
static const float DEBUG_TIME_REVERSE_FORWARD_SPEED = 50.f; // 1.0 = default speed

TAutoConsoleVariable CVar_WorldBuilderSpawnDebug(
	TEXT("WorldBuilder.SpawnDebug"),
	false,
	TEXT("Toggle debug for WorldBuilder spawning of resources, AI, etc."),
	ECVF_Cheat);

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

bool UWorldBuilderSubsystem::GenerateNewWorld(const FVector& Position, UWorldGenerationSpec* Spec, const struct FWorldGenerationActors& Actors, const struct FWorldGenerationTimeParams& TimeParams)
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
	FlushPersistentDebugLines(GetWorld());

	struct GridCellReserveSlot
	{
		GridCellReserveSlot(FIntPoint cellPosition, AActor* actor)
			: CellPosition(cellPosition), Actor(actor) {}
		FIntPoint CellPosition;
		AActor* Actor;
	};
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
					cellData.Actor = NULL;
				}

				// Random chance to spawn based on total actor density
				if (FMath::RandRange(0.f, 1.0f) <= WorldSpec->TotalCraftActorDensity)
				{
					// Determine if it's a tree or rock by seeing if a random number
					// falls within the density range of either one
					float randResourceIndex = FMath::RandRange(0.f, 1.0f);
					if (randResourceIndex <= realTreeDensity)
					{
						SpawnGridCellActor<ADestructableResourceActor>(
							WorldSpec->TreeActorClass,
							gridPosition,
							cellData,
							[](ADestructableResourceActor* SpawnedActor) {}
						);
					}
					else
					{
						SpawnGridCellActor<ADestructableResourceActor>(
							WorldSpec->RockActorClass,
							gridPosition,
							cellData,
							[](ADestructableResourceActor* SpawnedActor) {}
						);
					}
				}

				if (WorldSpec->SpawnEnemyAIEnabled && !WorldSpec->EnemyAISpecs.IsEmpty())
				{
					// Spawn random enemy
					if (FMath::RandRange(0.f, 1.0f) <= WorldSpec->EnemyAIDensity)
					{
						const int SpecIndex = FMath::RandRange(0, WorldSpec->EnemyAISpecs.Num() - 1);
						if (UAIEnemyCharacterSpec* EnemySpec = WorldSpec->EnemyAISpecs[SpecIndex])
						{
							// TODO : Spawn AI in grid
							SpawnGridCellActor<ACharacter>(
								EnemySpec->CharacterClass,
								gridPosition,
								cellData,
								[EnemySpec](ACharacter* SpawnedActor)
								{
									SpawnedActor->SpawnDefaultController();
									if (AEnemyAIController* EnemyController = Cast< AEnemyAIController >(SpawnedActor->GetController()))
									{
										if (EnemySpec->DefaultBehaviorTree != nullptr)
										{
											EnemyController->RunBehaviorTree(EnemySpec->DefaultBehaviorTree);
										}
									}
								}
							);
						}
						//EnemyAIClasses
					}
				}
			}
		}
	}
}

template< class TActorClass >
TActorClass* UWorldBuilderSubsystem::SpawnGridCellActor(const TSubclassOf<TActorClass>& ActorClass, const FIntPoint& GridPosition, GridCellData& OutCellData, std::function<void(TActorClass*)> InitializeFunc)
{
	if (!ensureMsgf(ActorClass != NULL, TEXT("[UWorldBuilderSubsystem::SpawnGridCellActor] Invalid Actor Class. Check your World Spec!")))
		return NULL;

	FVector GridOrigin = GeneratePosition;

	FActorSpawnParameters SpawnParameters;
	if (TActorClass* SpawnedActor = GetWorld()->SpawnActor<TActorClass>(ActorClass.Get(), SpawnParameters))
	{
		FVector CellTopLeft, CellBottomRight;
		GetGridCellBounds(GridPosition, CellTopLeft, CellBottomRight);

		// Position in random position within the cell
		float RandomX = FMath::RandRange(CellTopLeft.X, CellBottomRight.X);
		float RandomY = FMath::RandRange(CellTopLeft.Y, CellBottomRight.Y);

		FVector FinalPosition(RandomX, RandomY, GridOrigin.Z);

		// Random scale
		float ScaleAmount = FMath::RandRange(WorldSpec->CraftActorScaleMin, WorldSpec->CraftActorScaleMax);
		FVector Scale = FVector(ScaleAmount, ScaleAmount, ScaleAmount);
		SpawnedActor->SetActorScale3D(Scale);

		// Get actor bounds
		// TODO : This doesn't work for current resource actors because their static meshes have wrong pivots.
		//		  For now, override it with the logic just below.
		FVector ActorOrigin, ActorExtent;
		SpawnedActor->GetActorBounds(true, ActorOrigin, ActorExtent, false);

		// TODO BEGIN : Remove
		/*
		FBox StaticMeshBB;
		if (SpawnedActor->StaticMeshComponent)
		{
			
			if (UStaticMesh* ResourceMesh = SpawnedActor->StaticMeshComponent->GetStaticMesh())
			{
				
				StaticMeshBB = ResourceMesh->GetBoundingBox();
			}
		}
		ActorOrigin = StaticMeshBB.GetCenter();
		ActorExtent = StaticMeshBB.GetExtent();
		*/
		// TODO END : Remove

		// We've selected random X and Y, now raycast downwards to find the Z on the terrain
		FHitResult HitResult(ForceInit);
		if (PerformTerrainRaycast(FinalPosition, CELL_ACTOR_SPAWN_RAYCAST_LENGTH, HitResult))
		{
			// Offset the actor by its bounding box so it's bottom rests on the terrain
			const float OffsetFromCenter = SpawnedActor->GetActorLocation().Z - ActorOrigin.Z;
			const float HalfHeight = ActorExtent.Z;
			const float TotalOffset = (OffsetFromCenter + HalfHeight /* - CELL_ACTOR_SPAWN_OFFSET_Y*/) * ScaleAmount;
			if (HitResult.IsValidBlockingHit())
			{
				FinalPosition.Z = HitResult.ImpactPoint.Z + TotalOffset;
			}
		}

		SpawnedActor->SetActorLocation(FinalPosition);

		if (CVar_WorldBuilderSpawnDebug->GetBool())
		{
			DrawDebugSphere(GetWorld(), FinalPosition, 32.f, 16, FColor::Cyan, true, 0.f, 0, 2.5f);
			DrawDebugBox(GetWorld(), FinalPosition, ActorExtent, FColor::Cyan, true, 0.f, 0, 2.5f);
		}

		// Random yaw orientation
		FRotator rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
		SpawnedActor->SetActorRotation(rotation);

		InitializeFunc(SpawnedActor);

		// Update the cell data
		OutCellData.Actor = SpawnedActor;
		OutCellData.GridPosition = GridPosition;

		return SpawnedActor;
	}
	return nullptr;
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

	if (CVar_WorldBuilderSpawnDebug->GetBool())
	{
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, true, 0.f, 0, 5.f);
		if (bHit)
		{
			DrawDebugSphere(GetWorld(), hitResult.Location, 32.f, 32, FColor::Green, true, 0.f, 0, 5.f);
		}
	}

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

FIntPoint UWorldBuilderSubsystem::GetGridPosition(const FVector& inWorldPosition) const
{
	// Correct for divide by 0
	FVector worldPos = inWorldPosition;
	if (worldPos.X - worldPos.X == 0.f)
		worldPos.X = 1.f;
	if (worldPos.Y - worldPos.Y == 0.f)
		worldPos.Y = 1.f;

	const FVector gridCellSize(WorldSpec->GridCellSizeX, WorldSpec->GridCellSizeY, 1.f);
	const FVector offset = inWorldPosition - GeneratePosition;
	const FIntPoint gridPosition((int)((offset.X / gridCellSize.X)), (int)((offset.Y / gridCellSize.Y)));
	return gridPosition;
}

const UWorldBuilderSubsystem::GridCellData* UWorldBuilderSubsystem::FindGridCell(const FVector& inWorldPosition) const
{
	const FIntPoint gridPosition = GetGridPosition(inWorldPosition);
	if (gridPosition.X == -1 || gridPosition.Y == -1)
	{
		return NULL;
	}

	return &GridData[gridPosition.X][gridPosition.Y];
}

bool UWorldBuilderSubsystem::IsInGrid(const FIntPoint& inGridPos) const
{
	return GridData.Num() > 0
		&& inGridPos.X > -1 && inGridPos.Y > -1
		&& inGridPos.X < GridData[0].Num()
		&& inGridPos.Y < GridData.Num();
}

template< typename T >
T* UWorldBuilderSubsystem::FindClosestResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const
{
	FCollisionQueryParams TraceParams(FName(TEXT("FindClosestResourceActorTrace")), false);

	// Search around search location
	const float searchRadius = Range * 0.5f;
	FCollisionShape MySphere = FCollisionShape::MakeSphere(searchRadius); // 5M Radius
	TArray<FHitResult> outResults; outResults.Reserve(16);
	GetWorld()->SweepMultiByObjectType(outResults, SearchLocation, SearchLocation + FVector(searchRadius, searchRadius, 0.f), FQuat::Identity, ECC_WorldDynamic | CARAVAN_OBJECT_CHANNEL_INTERACTABLE, MySphere, TraceParams);

	T* outActor = NULL;
	float shortestDistance = TNumericLimits<float>::Max();

	for (const FHitResult& hit : outResults)
	{
		if (!IsValid(hit.GetActor()))
			continue;

		T* resourceActor = Cast< T >(hit.GetActor());
		if (resourceActor == NULL)
			continue;

		if (resourceActor->GetResourceType() != Type)
			continue;

		// Closest to instigator
		const float distance = FVector::Distance(InstigatorLocation, resourceActor->GetActorLocation());
		if (distance < shortestDistance)
		{
			outActor = resourceActor;
			shortestDistance = distance;
		}
	}

	return outActor;
}

ADestructableResourceActor* UWorldBuilderSubsystem::FindClosestDestructableResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const
{
	return FindClosestResourceActor< ADestructableResourceActor >(InstigatorLocation, SearchLocation, Range, Type);
}

APickupItemActor* UWorldBuilderSubsystem::FindClosestCraftResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const
{
	return FindClosestResourceActor< APickupItemActor >(InstigatorLocation, SearchLocation, Range, Type);
}

bool UWorldBuilderSubsystem::HasNearbyResourceActor(const FVector& SearchLocation, float Range) const
{
	// TODO : Ouch!
	for(ECraftResourceType Type : TEnumRange<ECraftResourceType>())
	{
		if( FindClosestDestructableResourceActor(SearchLocation, SearchLocation, Range, Type) ||
			FindClosestCraftResourceActor(SearchLocation, SearchLocation, Range, Type))
		{
			return true;
		}
	}
	return false;
}

FDataTableRowHandle UWorldBuilderSubsystem::GetResourceItemHandle(const FName& ItemRowName)
{
	FDataTableRowHandle OutHandle;
	OutHandle.DataTable = WorldSpec->ResourceItemsDataTable;
	OutHandle.RowName = ItemRowName;
	return OutHandle;
}

// Debug
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