// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"
#include "Tickable.h"
#include "AIRobotSubsystem.generated.h"

class ABaseAICharacter;
class ARobotAICharacter;
class ARobotAIController;
class UAIRobotCharacterSpec;
struct FRobotAIProfile;

/**
 * TODO : Rename to UAISubsystem, change to UTickableWorldSubsystem
 */
UCLASS()
class CARAVAN_API UAIRobotSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	// Begin FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UAIRobotSubsystem, STATGROUP_Tickables);
	}
	// End FTickableGameObject

	UFUNCTION(BlueprintCallable, Category = "AI|Spawn")
	ABaseAICharacter* SpawnEnemyCharacter(const UAIEnemyCharacterSpec* Spec, const FTransform& Transform);

	UFUNCTION(BlueprintCallable, Category = "AI|Spawn")
	ARobotAICharacter* SpawnRobotCharacter(const UAIRobotCharacterSpec* Spec, const FTransform& Transform);

	UFUNCTION(BlueprintCallable, Category = "AI|Spawn")
	bool DespawnRobotCharacter(ARobotAICharacter* RobotCharacter);


	UPROPERTY(Transient, BlueprintReadOnly, Category = "AI")
	TArray< ARobotAIController* > RobotControllers;

private:
	bool BuildAIProfileFromSpec(const UAIRobotCharacterSpec* Spec, FRobotAIProfile& OutProfile) const;
	void InitializeRobot(const UAIRobotCharacterSpec* Spec, ARobotAICharacter* Character, ARobotAIController* Controller) const;
	void RegisterRobot(ARobotAIController* Controller);
	void UnregisterRobot(ARobotAIController* Controller);
};
