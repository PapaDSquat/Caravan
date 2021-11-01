// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAICharacter.h"
#include "AI/RobotAIController.h"
#include "AI/AICharacterSpec.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "Utils/CaravanEngineUtils.h"

static TAutoConsoleVariable<bool> CVarAIDebug_Profile(
	TEXT("AI.Robot.Profile"),
	false,
	TEXT("Toggle AI Profile HUD Overlay"),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarAIDebug_Behaviour(
	TEXT("AI.Robot.Behaviour"),
	false,
	TEXT("Toggle AI Behaviour HUD Overlay"),
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
	{
		const FVector rootPos = GetActorLocation();
		const FVector right = GetActorRightVector();
		const FVector up = GetActorUpVector();

		static float c_DebugXOffset = 150.f;
		static float c_DebugYOffset = 30.f;

		auto DrawAIPropertyString = [&](int index, const FString& string)
		{
			const float offsetX = c_DebugXOffset;
			const float offsetY = (-c_DebugYOffset * index);

			const FVector stringOffset = (FVector(offsetX, offsetX, 0.f) * right) + (FVector(0.f, 0.f, offsetY) * up);
			DrawDebugString(
				GetWorld(),
				stringOffset,
				string,
				this,
				FColor::Green,
				0.f
			);
		};

		int idx = 0;
		if (CVarAIDebug_Profile.GetValueOnGameThread() == true)
		{
			DrawAIPropertyString(idx++, controller->GetRobotName().ToString());
			DrawAIPropertyString(idx++, CaravanUtils::EnumToString(controller->CharacterProfile.PrimarySkill));
		}
		if (CVarAIDebug_Behaviour.GetValueOnGameThread() == true)
		{
			DrawAIPropertyString(idx++, controller->GetBrainComponent()->GetDebugInfoString());
		}
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
