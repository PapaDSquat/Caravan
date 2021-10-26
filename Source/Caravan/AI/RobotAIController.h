// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

/**
 * 
 */
UCLASS()
class CARAVAN_API ARobotAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
