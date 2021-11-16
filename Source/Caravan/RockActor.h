// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DestructableResourceActor.h"
#include "RockActor.generated.h"

UCLASS()
class ARockActor : public ADestructableResourceActor
{
	GENERATED_BODY()
	
public:
	ARockActor(const class FObjectInitializer& ObjInitializer);
};
