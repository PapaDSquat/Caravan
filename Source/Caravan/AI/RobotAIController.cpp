// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RobotAIController.h"
#include "AI/RobotAICharacter.h"
#include "AI/AIRobotSubsystem.h"
#include "Components/InteractableComponent.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

ARobotAIController::ARobotAIController(const class FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer)
{
	FindOrCreateComponent(UInventoryComponent, InventoryComponent, "InventoryComponent")
	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->PrimaryInteractionName = FText::FromString("Talk");
	}
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Event Registration
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::OnUnPossess()
{
	// Event Unregistration
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.RemoveDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARobotAIController::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	// TODO	
}

ARobotAICharacter* ARobotAIController::GetRobotOwner() const
{
	return Cast< ARobotAICharacter >(GetOwner());
}

FName ARobotAIController::GetRobotName() const
{
	return CharacterProfile.Name;
}

void ARobotAIController::SetCurrentAILocale(ERobotAILocale newLocale)
{
	CurrentAILocale = newLocale;
}