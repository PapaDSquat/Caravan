// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"
#include "AIRobotSubsystem.generated.h"

class ARobotAIController;
class UAIRobotCharacterSpec;
struct FRobotAIProfile;

/**
 * 
 */
UCLASS()
class CARAVAN_API UAIRobotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	bool BuildCharacterFromSpec(const UAIRobotCharacterSpec* Spec, FRobotAIProfile& OutProfile) const;
	void RegisterRobot(const ARobotAIController* robotController);

private:
	struct FAIRobotInternalData
	{
		FName Name;
	};
	TArray< FAIRobotInternalData > RegisteredRobots;
};
