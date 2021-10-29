// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIRobotSubsystem.h"
#include "AI/AICharacterSpec.h"
#include "AI/RobotAICharacter.h"

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
			const int32 Index = NamesPool.IndexOfByKey(RegisteredRobot.Name);
			if (Index != INDEX_NONE)
			{
				NamesPool.RemoveAt(Index);
			}
		}
		
		// Pick random name from pool
		PickRandom(NamesPool, OutProfile.Name);
	}

	return true;
}

void UAIRobotSubsystem::RegisterRobot(const ARobotAICharacter* RobotCharacter)
{
	if (!ensure(RobotCharacter != nullptr))
		return;

	FAIRobotInternalData& Data = RegisteredRobots.Emplace_GetRef();
	Data.Name = RobotCharacter->GetRobotName();
}