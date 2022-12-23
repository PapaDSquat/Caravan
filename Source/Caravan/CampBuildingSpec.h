#pragma once

#include "AI/AICharacterSpec.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "CampBuildingSpec.generated.h"

class ACampBuildingActor;

UENUM(BlueprintType)
enum class ECampBuildingType : uint8
{
    Woodwork,
    Invalid
};

UCLASS(BlueprintType)
class UCampBuildingSpec : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECampBuildingType BuildingType = ECampBuildingType::Invalid;

    // Actor class associated with this building
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACampBuildingActor> BuildingActorClass;

    // Skill required by AI to work here
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERobotAISkill WorkerAISkill = ERobotAISkill::Invalid;
};