// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGenerator.h"
#include "WorldBuilder/WorldBuilderSubsystem.h"

AWorldGenerator::AWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (UWorldBuilderSubsystem* WorldBuilderSubsystem = GameInstance->GetSubsystem<UWorldBuilderSubsystem>())
	{
		FWorldGenerationTimeParams TimeParams = InitialTimeParams;
		if (DEBUG_RandomTimeOfDayOnStart)
		{
			TimeParams.InitialTimeOfDayRatio = FMath::RandRange(0.f, 1.f);
		}

		WorldBuilderSubsystem->GenerateNewWorld(GetActorLocation(), WorldGenerationSpec, WorldActors, InitialTimeParams);
	}
}

void AWorldGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (UWorldBuilderSubsystem* WorldBuilderSubsystem = GameInstance->GetSubsystem<UWorldBuilderSubsystem>())
	{
		WorldBuilderSubsystem->UpdateTime(DeltaTime);
	}
}
