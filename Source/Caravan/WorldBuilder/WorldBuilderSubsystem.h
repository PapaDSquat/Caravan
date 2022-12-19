// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldBuilderSubsystem.generated.h"

class ACaravanActor;
class ACaravanCharacter;
class UWorldGenerationSpec;
enum class ECraftResourceType : uint8;

UCLASS()
class CARAVAN_API UWorldBuilderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	// TODO: Move this into a tickable Time System
	void UpdateTime(float DeltaTime);

	bool GenerateNewWorld(const FVector& Position, UWorldGenerationSpec* Spec, const struct FWorldGenerationActors& Actors, const struct FWorldGenerationTimeParams& TimeParams);
	void ResetResourceGrid();

	UFUNCTION(BlueprintCallable)
	UWorldGenerationSpec* GetWorldSpec() const { return WorldSpec; }

	/*
		TODO
			MaxRange should not have default value
			Rename MaxRange to radius and actually treat it as such!!
			Overload for Position and Radius
			Move Pos & Radius as first 2 params. Type will eventually change
	*/

	template< typename T >
	T* FindClosestResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const;

	// Helpers
	UFUNCTION(BlueprintCallable)
	class ADestructableResourceActor* FindClosestDestructableResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const;
	
	UFUNCTION(BlueprintCallable)
	class ACraftResourceActor* FindClosestCraftResourceActor(const FVector& InstigatorLocation, const FVector& SearchLocation, float Range, ECraftResourceType Type) const;

	bool HasNearbyResourceActor(const FVector& SearchLocation, float Range) const;

	//template< typename T >
	//T* FindClosestResourceActor(const FVector& SearchActor, ECraftResourceType Type, float MaxRange = -1.f);

	// Time
	UFUNCTION(BlueprintCallable)
	FString GetCurrentTimeString() const;

	UPROPERTY(BlueprintReadOnly)
	int CurrentDay = 1;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSunPitch = 0;

	// DEBUG flag to actively reverse time
	UPROPERTY(BlueprintReadWrite)
	bool DEBUG_TimeReverse = false;

	// DEBUG flag to actively forward time
	UPROPERTY(BlueprintReadWrite)
	bool DEBUG_TimeForward = false;

private:
	struct GridCellData
	{
		AActor* Actor{ NULL };
		FIntPoint GridPosition;
	};
	TArray< TArray<GridCellData> > GridData;

	template< class TActorClass >
	TActorClass* SpawnGridCellActor(const TSubclassOf<TActorClass>& ActorClass, GridCellData& CellData, const FIntPoint& GridPosition);

	bool PerformTerrainRaycast(const AActor* Actor, FHitResult& hitResult);
	bool PerformTerrainRaycast(const FVector& traceStart, float length, FHitResult& hitResult);

	// Grid Helpers
	void GetGridCellBounds(const FIntPoint& cellPosition, FVector& topLeft, FVector& bottomRight) const;
	FIntPoint GetGridPosition(const FVector& inWorldPosition) const;
	const GridCellData* FindGridCell(const FVector& inWorldPosition) const;
	bool IsInGrid(const FIntPoint& inGridPos) const;

	FVector GeneratePosition;
	UWorldGenerationSpec* WorldSpec = NULL;
	ACaravanCharacter const * PlayerCharacter = NULL;
	ACaravanActor const * CaravanActor = NULL;

	// Time
	float SecondsPerDay = 180;
	int TimeDisplayMinuteInterval = 15;

	float CurrentDayElapsedTimeSeconds{ 0.f };
	float CurrentTimeSpeed{ 1.0f };
	
};
