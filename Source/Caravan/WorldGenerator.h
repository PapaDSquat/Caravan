// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

class ACaravanCharacter;
class ACaravanActor;

UCLASS()
class CARAVAN_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void ResetResourceGrid();


	// TIME PROPERTIES

	// Number of seconds that make up a full game day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings")
	float SecondsPerDay = 180;

	// Initial time of day on start of game, between 0 and 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings")
	float InitialTimeOfDayRatio = 0.5f; // Between 0 and 1

	// Interval of minutes before updating the time UI display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings")
	int TimeDisplayMinuteInterval = 15;

	UPROPERTY(BlueprintReadOnly, Category = "Time Data")
	int CurrentDay = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Time Data")
	float CurrentSunPitch = 0;

	UFUNCTION(BlueprintCallable, Category = "Time Data")
	FString GetCurrentTimeString() const;

	// DEBUG speed for reverse/forward where 1.0 is default speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings DEBUG")
	float DEBUG_ReverseForwardSpeed = 50.f; // 1.0 = default speed

	// DEBUG flag to start the game at a random time of day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings DEBUG")
	bool DEBUG_TimeReverse = false;

	// DEBUG flag to start the game at a random time of day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings DEBUG")
	bool DEBUG_TimeForward = false;

	// DEBUG flag to start the game at a random time of day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings DEBUG")
	bool DEBUG_RandomTimeOfDayOnStart = false;

	// GRID GENERATION PROPERTIES

	// Toggle whether to spawn trees, rocks, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	bool SpawnResourceActorsEnabled = true;

	// Toggle whether to spawn Player, Caravan, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	bool SpawnGameActorsEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int GridCellCountX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int GridCellCountY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float GridCellSizeX = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float GridCellSizeY = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Crafting")
	float TotalCraftActorDensity = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Crafting")
	float TreeDensity = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Crafting")
	float RockDensity = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Crafting")
	float CraftActorScaleMin = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Crafting")
	float CraftActorScaleMax = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Actors")
	ACaravanCharacter* PlayerCharacter = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Actors")
	FIntPoint PlayerCell = FIntPoint(-1, -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Actors")
	ACaravanActor* CaravanActor = NULL;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Actors")
	FIntPoint CaravanCell = FIntPoint(-1, -1);

private:
	struct GridCellData
	{
		AActor* Actor{ NULL };
	};
	struct GridCellReserveSlot
	{
		GridCellReserveSlot(FIntPoint cellPosition, AActor* actor)
			: CellPosition(cellPosition), Actor(actor) {}
		FIntPoint CellPosition;
		AActor* Actor;
	};

	TArray< TArray<GridCellData> > GridData;

	template< class ActorClass >
	ActorClass* SpawnGridCellActor(GridCellData& cellData, const FName& meshName, const FIntPoint& gridPosition);

	bool PerformTerrainRaycast(const AActor* Actor, FHitResult& hitResult);
	bool PerformTerrainRaycast(const FVector& traceStart, float length, FHitResult& hitResult);

	void GetGridCellBounds(const FIntPoint& cellPosition, FVector& topLeft, FVector& bottomRight) const;

	// Time
	float CurrentDayElapsedTimeSeconds{ 0.f };
	float CurrentTimeSpeed{ 1.0f };
};
