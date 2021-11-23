// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldBuilder/CharacterSpawner.h"
#include "AI/AICharacterSpec.h"
#include "GameFramework/Character.h"
#include "Utils/CaravanSpawnUtils.h"

ACharacterSpawner::ACharacterSpawner()
{

}

void ACharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnedActor = Spawn();
	}
}

AActor* ACharacterSpawner::Spawn()
{
	return CaravanSpawnUtils::SpawnActor(this, CharacterSpec, GetActorLocation(), GetActorRotation());
}