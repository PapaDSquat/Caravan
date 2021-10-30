// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotAICharacter.generated.h"

UENUM(BlueprintType)
enum class ERobotAILocale : uint8
{
	CaravanInterior,
	CaravanCamp,
	PlayerExpedition,
	// PersonalExpedition  // Exploring on it's own?
};

UENUM(BlueprintType)
enum class ERobotAISkill : uint8
{
	Carpenter,				// Collect wood, craft wood items
	// Blacksmith,			// Collect ore, craft weapons
	// Doctor,				// Heal team, craft healing items
	// Alchemist,			// Magic support, craft potions
	// Cartographer,		// Expand player map, discover POIs and secrets
	// Chef,				// Collect plants, cook food items
	// Tailor,				// Collect materials, craft clothing
	// Decorator,			// Collect materials, build camp infrastructure
	// Intern				// All around worker
};

UENUM(BlueprintType)
enum class ERobotAIQuirk : uint8
{
	Perfectionist,
	// NightOwl,
	// AdrenalineJunkie,
	// Glutton,
	// AlmostBlind,
	// Clumsy
	// TODO
};

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

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	ERobotAILocale CurrentLocale;
};
