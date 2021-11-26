// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RobotAIController.h"
#include "AI/RobotAICharacter.h"
#include "AI/AIRobotSubsystem.h"
#include "Components/InteractableComponent.h"
#include "Utils/CaravanEngineUtils.h"

ARobotAIController::ARobotAIController(const FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer)
{
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!RobotCharacter)
		return;

	// Event Registration
	if (IsValid(RobotCharacter->InteractableComponent))
	{
		RobotCharacter->InteractableComponent->OnInteract.AddDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!RobotCharacter)
		return;

	// Event Unregistration
	if (IsValid(RobotCharacter->InteractableComponent))
	{
		RobotCharacter->InteractableComponent->OnInteract.RemoveDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARobotAIController::OnRegisterToSubsystem()
{
	OnAIRobotRegister.Broadcast(this);

	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!RobotCharacter)
		return;

	// Setup Interactable
	if (IsValid(RobotCharacter->InteractableComponent))
	{
		RobotCharacter->InteractableComponent->InteractableObjectName = FText::FromName(CharacterProfile.Name);
		RobotCharacter->InteractableComponent->InteractableObjectSubTitle = FText::FromString(CaravanUtils::EnumToString(CharacterProfile.PrimarySkill));
	}
}

void ARobotAIController::OnUnregisterFromSubsystem()
{
	OnAIRobotUnregister.Broadcast(this);
}

void ARobotAIController::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	// TODO	
}

ARobotAICharacter* ARobotAIController::GetRobotOwner() const
{
	return Cast< ARobotAICharacter >(GetCharacter());
}

FName ARobotAIController::GetRobotName() const
{
	return CharacterProfile.Name;
}

ERobotAILocale ARobotAIController::GetCurrentAILocale() const
{
	return RobotState.Locale;
}

void ARobotAIController::SetCurrentAILocale(ERobotAILocale newLocale)
{
	RobotState.Locale = newLocale;
}

bool ARobotAIController::GetIsOnExpedition() const
{
	return RobotState.bFollowPlayerRequested;
}

void ARobotAIController::SetIsOnExpedition(bool bIsOnExpedition)
{
	RobotState.bFollowPlayerRequested = bIsOnExpedition;
}