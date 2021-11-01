// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacterSpec.h"
#include "GameFramework/Character.h"
#include "RobotAICharacter.generated.h"

class ARobotAIController;

UCLASS()
class CARAVAN_API ARobotAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARobotAICharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Robot")
	ARobotAIController* GetRobotController() const;

	UFUNCTION(BlueprintCallable, Category = "Robot")
	FName GetRobotName() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ShowOnlyInnerProperties))
	UAIRobotCharacterSpec* CharacterSpec;
};
