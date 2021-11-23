// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIRobotSubsystem.h"
#include "AI/AICharacterSpec.h"
#include "AI/RobotAICharacter.h"
#include "AI/RobotAIController.h"
#include "Utils/CaravanSpawnUtils.h"

template< typename T >
int32 PickRandom(const TArray< T >& Source, T& Out)
{
	if (Source.Num() == 0) 
		return -1;

	const int32 RandIndex = FMath::RandRange(0, Source.Num() - 1);
	Out = Source[RandIndex];
	return RandIndex;
}

void UAIRobotSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void UAIRobotSubsystem::Deinitialize()
{

}

void UAIRobotSubsystem::Tick(float DeltaTime)
{
	// TODO
}

ARobotAICharacter* UAIRobotSubsystem::SpawnRobotCharacter(const UAIRobotCharacterSpec* Spec, const FTransform& Transform)
{
	ARobotAICharacter* SpawnedAICharacter = Cast< ARobotAICharacter >(CaravanSpawnUtils::SpawnActor(GetWorld(), Spec, Transform));
	if (SpawnedAICharacter)
	{
		ARobotAIController* RobotController = Cast< ARobotAIController >(SpawnedAICharacter->GetController());
		if (ensure(RobotController))
		{
			BuildCharacterFromSpec(Spec, RobotController->CharacterProfile);
			RegisterRobot(RobotController);
		}
	}
	return SpawnedAICharacter;
}

bool UAIRobotSubsystem::DespawnRobotCharacter(ARobotAICharacter* RobotCharacter)
{
	bool bSuccess = false;
	if (RobotCharacter)
	{
		ARobotAIController* RobotController = Cast< ARobotAIController >(RobotCharacter->GetController());
		if (ensure(RobotController))
		{
			UnregisterRobot(RobotController);
		}
		bSuccess = RobotCharacter->Destroy();
	}
	return bSuccess;
}

bool UAIRobotSubsystem::BuildCharacterFromSpec(const UAIRobotCharacterSpec* Spec, FRobotAIProfile& OutProfile) const
{
	if (Spec == nullptr)
		return false;

	// Name
	{
		TArray< FName > NamesPool = Spec->Names;
		
		// Remove any already used names
		for (const FAIRobotInternalData& RegisteredRobot : RegisteredRobots)
		{
			if (!RegisteredRobot.Controller)
				continue;

			const int32 Index = NamesPool.IndexOfByKey(RegisteredRobot.Controller->GetRobotName());
			if (Index != INDEX_NONE)
			{
				NamesPool.RemoveAt(Index);
			}
		}
		
		// Pick random name from pool
		PickRandom(NamesPool, OutProfile.Name);
	}

	// Skills
	{
		OutProfile.PrimarySkill = Spec->PrimarySkill;
	}

	// Crafting
	{
		OutProfile.PreferredCraftResource = Spec->PreferredCraftResource;
	}

	return true;
}

void UAIRobotSubsystem::RegisterRobot(ARobotAIController* Controller)
{
	if (!ensure(Controller))
		return;

	FAIRobotInternalData& Data = RegisteredRobots.Emplace_GetRef();
	Data.Controller = Controller;
}

void UAIRobotSubsystem::UnregisterRobot(ARobotAIController* Controller)
{
	if (!ensure(Controller))
		return;
	
	int FoundIndex = RegisteredRobots.IndexOfByPredicate(
		[Controller](const FAIRobotInternalData& Data) { return Data.Controller == Controller; });
	if (FoundIndex != INDEX_NONE)
	{
		RegisteredRobots.RemoveAt(FoundIndex);
	}
}