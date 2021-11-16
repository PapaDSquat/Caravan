// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableActor.h"
#include "Caravan.h"
#include "Engine.h"

AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

AInteractableActor::~AInteractableActor()
{
}

void AInteractableActor::Tick(float DeltaSeconds)
{
	if (IsFocused)
	{
		//GEngine->AddOnScreenDebugMessage(0, -1.f, FColor::Green, GetInteractionName());
	}
}
/*
void AInteractableActor::OnInteractFocus(const InteractData& interactData)
{
	if (!IsFocused)
	{
		IsFocused = true;
		NotifyOnInteractFirstFocus();
	}
}

void AInteractableActor::OnInteractUnFocus(const InteractData& interactData)
{
	IsFocused = false;
	NotifyOnInteractUnFocus();
}

EInteractionType AInteractableActor::OnInteractSelect(const InteractData& interactData)
{
	NotifyOnInteractSelect();
	return EInteractionType::None;
}
*/

void AInteractableActor::Interact(APawn* interactingPawn)
{
	//InteractData data(interactingPawn);
	//OnInteractSelect(data);
}