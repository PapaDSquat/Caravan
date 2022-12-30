#pragma once

#include "AI/AICharacterSpec.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BaseAICharacter.generated.h"

class UInteractableComponent;
class UInventoryComponent;

UCLASS()
class CARAVAN_API ABaseAICharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	ABaseAICharacter();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	UInventoryComponent* InventoryComponent;
};
