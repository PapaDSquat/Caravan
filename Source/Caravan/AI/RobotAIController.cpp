// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RobotAIController.h"
#include "AI/RobotAICharacter.h"
#include "AI/AIRobotSubsystem.h"
#include "RPG/Inventory.h"

ARobotAIController::ARobotAIController()
{
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
}

void ARobotAIController::BeginPlay()
{
	Super::BeginPlay();

	Inventory = NewObject<UInventory>();
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

ARobotAICharacter* ARobotAIController::GetRobotOwner() const
{
	return Cast< ARobotAICharacter >(GetOwner());
}

FName ARobotAIController::GetRobotName() const
{
	return CharacterProfile.Name;
}
