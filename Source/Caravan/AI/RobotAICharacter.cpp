// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAICharacter.h"
#include "AI/AICharacterSpec.h"
#include "AI/AIRobotSubsystem.h"
#include "DrawDebugHelpers.h"

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
	
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (UAIRobotSubsystem* AIRobotSubsystem = GameInstance->GetSubsystem<UAIRobotSubsystem>())
	{
		// TODO : Saved profile
		if (AIRobotSubsystem->BuildCharacterFromSpec(CharacterSpec, CharacterProfile))
		{
			AIRobotSubsystem->RegisterRobot(this);
		}
	}
}

// Called every frame
void ARobotAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CVarAIDebug.GetValueOnGameThread() == true)
	{
		/* constexpr */ static FVector c_DebugTextOffset(0.f, 0.f, 100.f);
		DrawDebugString(
			GetWorld(),
			c_DebugTextOffset,
			CharacterProfile.Name.ToString(),
			this,
			FColor::Green,
			0.f
		);
	}
	//
}

// Called to bind functionality to input
void ARobotAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FName ARobotAICharacter::GetRobotName() const
{
	return CharacterProfile.Name;
}
