// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldGenerationSpec.generated.h"

class ACaravanActor;
class ACaravanCharacter;
class ACraftResourceActor;
class ADestructableResourceActor;
class UAIEnemyCharacterSpec;
class UDataTable;

USTRUCT(BlueprintType)
struct FWorldGenerationActors
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	ACaravanCharacter* PlayerCharacter = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FIntPoint PlayerCell = FIntPoint(-1, -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan")
	ACaravanActor* CaravanActor = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan")
	FIntPoint CaravanCell = FIntPoint(-1, -1);
};

USTRUCT(BlueprintType)
struct FWorldGenerationTimeParams
{
	GENERATED_USTRUCT_BODY()

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
};

UCLASS()
class CARAVAN_API UWorldGenerationSpec : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	TObjectPtr<const UDataTable> ResourceItemsDataTable;

	// TODO : Get rid of this crafting shit
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	TSubclassOf<ADestructableResourceActor> TreeActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	TSubclassOf<ADestructableResourceActor> RockActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	TSubclassOf<ACraftResourceActor> WoodActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	TSubclassOf<ACraftResourceActor> StoneActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TArray<UAIEnemyCharacterSpec*> EnemyAISpecs;

	// Toggle whether to spawn trees, rocks, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool SpawnResourceActorsEnabled = true;

	// Toggle whether to spawn Player, Caravan, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool SpawnGameActorsEnabled = true;

	// Toggle whether to spawn Enemy AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool SpawnEnemyAIEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int GridCellCountX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int GridCellCountY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float GridCellSizeX = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float GridCellSizeY = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float TotalCraftActorDensity = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float TreeDensity = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float RockDensity = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float CraftActorScaleMin = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float CraftActorScaleMax = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float EnemyAIDensity = 0.2f;
};
