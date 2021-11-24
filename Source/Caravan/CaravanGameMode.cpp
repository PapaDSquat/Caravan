// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaravanGameMode.h"
#include "Caravan.h"
#include "CaravanActor.h"
#include "CaravanCharacter.h"

ACaravanGameMode::ACaravanGameMode()
{
}

void ACaravanGameMode::BeginPlay()
{
	Super::BeginPlay();

	CaravanActor = Cast<ACaravanActor>(UGameplayStatics::GetActorOfClass(GetWorld(), CaravanActorClass));
}