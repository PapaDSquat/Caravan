// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WorldBuilder/WorldGenerationSpec.h"
#include "WorldGenerator.generated.h"

UCLASS()
class CARAVAN_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldGenerator();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generation")
	UWorldGenerationSpec* WorldGenerationSpec;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generation")
	FWorldGenerationActors WorldActors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generation")
	FWorldGenerationTimeParams InitialTimeParams;

	// DEBUG flag to start the game at a random time of day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEBUG")
	bool DEBUG_RandomTimeOfDayOnStart = false;
};
