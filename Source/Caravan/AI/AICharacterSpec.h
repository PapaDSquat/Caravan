// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AICharacterSpec.generated.h"

/**
* Create UDataAsset of this type in Editor to fill with themes
*/
UCLASS()
class CARAVAN_API UAICharacterSpec : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< FName > Names;
};

UCLASS()
class CARAVAN_API UAIRobotCharacterSpec : public UAICharacterSpec
{
	GENERATED_BODY()

public:
	// TODO
	// Can effect look, speed, dialogue
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	// int8 Age;

	// Skills already known and already able to perform well
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	// UAISkill PrimarySkill;

	// Skills that can be learned and advanced in order to perform new tasks and change primary job
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	// TArray< UAISkill > SecondarySkills;

	// General temperament of the robot in dialogue
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	// TArray< UAITrait > Traits;

	// Traits have positive & negative implications on robot’s AI behaviour
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	// TArray< UAIQuirk > Quirks;
};