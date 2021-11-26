// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftResourceActor.h"
#include "Engine/DataAsset.h"
#include "AICharacterSpec.generated.h"

UENUM(BlueprintType)
enum class ERobotAISkill : uint8
{
	Carpenter,				// Collect wood, craft wood items
	Blacksmith,				// Collect ore, craft weapons
	// Doctor,				// Heal team, craft healing items
	// Alchemist,			// Magic support, craft potions
	// Cartographer,		// Expand player map, discover POIs and secrets
	// Chef,				// Collect plants, cook food items
	// Tailor,				// Collect materials, craft clothing
	// Decorator,			// Collect materials, build camp infrastructure
	// Intern				// All around worker
	Invalid
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
	Invalid
};

// TODO : Rename to EWorldLocale - used by Player as well, potentionally by other Objects
UENUM(BlueprintType)
enum class ERobotAILocale : uint8
{
	CaravanInterior,
	CaravanCamp,
	PlayerExpedition,
	// PersonalExpedition  // Exploring on it's own?
	Invalid
};

USTRUCT(BlueprintType)
struct CARAVAN_API FRobotAIProfile
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FName Name = "INVALID NAME";

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	ERobotAISkill PrimarySkill = ERobotAISkill::Invalid;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	ECraftResourceType PreferredCraftResource = ECraftResourceType::Invalid;
};

// TODO: Move out of here
UCLASS()
class CARAVAN_API UCharacterSpec : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	TSubclassOf< class ACharacter > CharacterClass;
};

/**
* Create UDataAsset of this type in Editor to fill with themes
*/
UCLASS()
class CARAVAN_API UAICharacterSpec : public UCharacterSpec
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan")
	TArray< FName > Names;
};

UCLASS()
class CARAVAN_API UAIRobotCharacterSpec : public UAICharacterSpec
{
	GENERATED_BODY()

public:
	// TODO
	// Can effect look, speed, dialogue
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	// int8 Age;

	// Skills already known and already able to perform well
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	ERobotAISkill PrimarySkill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	ECraftResourceType PreferredCraftResource;

	// Skills that can be learned and advanced in order to perform new tasks and change primary job
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	// TArray< ERobotAISkill > SecondaryInterests;

	// General temperament of the robot in dialogue
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Caravan Robot")
	// TArray< ERobotAITrait > Traits;

	// Traits have positive & negative BlueprintReadOnly on robot’s AI behaviour
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caravan Robot")
	// TArray< ERobotAIQuirk > Quirks;
};