// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSpawner.generated.h"

class UCharacterSpec;

UCLASS()
class CARAVAN_API ACharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterSpawner();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* Spawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UCharacterSpec* CharacterSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bSpawnOnBeginPlay = true;

private:
	UPROPERTY()
	AActor* SpawnedActor = NULL;
};
