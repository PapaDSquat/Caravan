// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RobotAIController.h"
#include "AI/RobotAICharacter.h"
#include "AI/AIRobotSubsystem.h"
#include "Components/InteractableComponent.h"
#include "RPG/InventoryComponent.h"

ARobotAIController::ARobotAIController()
{
	InventoryComponent = FindComponentByClass< UInventoryComponent >();
	if (InventoryComponent == NULL)
	{
		InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	}

	InteractableComponent = FindComponentByClass< UInteractableComponent >();
	if (InteractableComponent == NULL)
	{
		InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	}
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ARobotAICharacter* robotCharacter = Cast< ARobotAICharacter >(InPawn);
	if (robotCharacter == NULL)
	{
		return;
	}

	BuildCharacterFromSpec(robotCharacter->CharacterSpec);

	// Event Registration
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::OnUnPossess()
{
	// Event Unregistration
}

void ARobotAIController::BeginPlay()
{
	Super::BeginPlay();
}

bool ARobotAIController::BuildCharacterFromSpec(const UAIRobotCharacterSpec* characterSpec)
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (UAIRobotSubsystem* AIRobotSubsystem = GameInstance->GetSubsystem<UAIRobotSubsystem>())
	{
		// TODO : Saved profile
		if (AIRobotSubsystem->BuildCharacterFromSpec(characterSpec, CharacterProfile))
		{
			AIRobotSubsystem->RegisterRobot(this);
			return true;
		}
	}
	return false;
}

void ARobotAIController::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	
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