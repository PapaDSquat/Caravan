// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotAICharacter.generated.h"

USTRUCT(BlueprintType)
struct CARAVAN_API FRobotAIProfile
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FName Name = "INVALID NAME";
};

UCLASS()
class CARAVAN_API ARobotAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARobotAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Robot")
	FName GetRobotName() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ShowOnlyInnerProperties))
	class UAIRobotCharacterSpec* CharacterSpec;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FRobotAIProfile CharacterProfile;
};
