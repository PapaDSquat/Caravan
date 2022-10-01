// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAICharacter.h"
#include "AI/RobotAIController.h"
#include "AI/AICharacterSpec.h"
#include "BrainComponent.h"
#include "Components/InteractableComponent.h"
#include "Debug/CaravanConsoleVariables.h"
#include "DrawDebugHelpers.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

ARobotAICharacter::ARobotAICharacter(const FObjectInitializer& ObjInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	FindOrCreateComponent(UInventoryComponent, InventoryComponent, "InventoryComponent")
	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->PrimaryInteractionName = FText::FromString("Talk");
		InteractableComponent->SetupAttachment(RootComponent);
	}
}

void ARobotAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

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
			if (UBrainComponent* brainComponent = controller->GetBrainComponent())
			{
				DrawAIPropertyString(idx++, brainComponent->GetDebugInfoString());
			}
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
