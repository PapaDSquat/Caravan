// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DestructableResourceActor.h"
#include "TreeActor.generated.h"

UCLASS()
class ATreeActor : public ADestructableResourceActor
{
	GENERATED_BODY()

public:
	ATreeActor(const class FObjectInitializer& ObjInitializer);
};
