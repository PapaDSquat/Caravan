// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "CaravanGameMode.generated.h"

class ACaravanActor;

UCLASS(minimalapi)
class ACaravanGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACaravanGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Caravan")
	ACaravanActor* CaravanActor = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf< ACaravanActor > CaravanActorClass;
};



