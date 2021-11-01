// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAICharacter.h"
#include "AI/RobotAIController.h"
#include "AI/AICharacterSpec.h"
#include "DrawDebugHelpers.h"
#include "Utils/CaravanEngineUtils.h"

static TAutoConsoleVariable<bool> CVarAIDebug(
	TEXT("AI.Robot.Spec"),
	false,
	TEXT("Toggle AI debug HUD Overlay"),
	ECVF_Cheat);

// Sets default values
ARobotAICharacter::ARobotAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARobotAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARobotAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ARobotAIController* controller = GetRobotController();
	if (controller == NULL)
	{
		return;
	}
	
	// DEBUG
	// TODO: Fix positioning
	if (CVarAIDebug.GetValueOnGameThread() == true)
	{
		static FVector c_DebugNameOffset(150.f, 0.f, 100.f);
		DrawDebugString(
			GetWorld(),
			c_DebugNameOffset,
			controller->GetRobotName().ToString(),
			this,
			FColor::Green,
			0.f
		);

		static FVector c_DebugPrimarySkillOffset(150.f, -50.f, 100.f);
		DrawDebugString(
			GetWorld(),
			c_DebugPrimarySkillOffset,
			CaravanUtils::EnumToString(controller->CharacterProfile.PrimarySkill),
			this,
			FColor::Green,
			0.f
		);
	}
}

// Called to bind functionality to input
void ARobotAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

ARobotAIController* ARobotAICharacter::GetRobotController() const
{
	return Cast< ARobotAIController >(GetController());
}

FName ARobotAICharacter::GetRobotName() const
{
	if (ARobotAIController* controller = GetRobotController())
	{
		return controller->GetRobotName();
	}
	return NAME_None;
}
