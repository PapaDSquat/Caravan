#pragma once

#include "AI/AICharacterSpec.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BaseAICharacter.generated.h"

class UHealthComponent;
class UInteractableComponent;
class UInventoryComponent;

UCLASS()
class CARAVAN_API ABaseAICharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;

	UHealthComponent* GetHealthComponent() { return HealthComponent; }
	UInteractableComponent* GetInteractableComponent() { return InteractableComponent; }
	UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

protected:
	UFUNCTION()
	void OnKill(UHealthComponent* InHealthComponent, AActor* InInstigator);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	UInventoryComponent* InventoryComponent;
};
