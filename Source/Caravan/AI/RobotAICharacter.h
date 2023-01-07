#pragma once

#include "AI/BaseAICharacter.h"
#include "AI/AICharacterSpec.h"
#include "CoreMinimal.h"

#include "RobotAICharacter.generated.h"

class ARobotAIController;
class UInteractableComponent;
class UInteractionComponent;
class UInventoryComponent;

UCLASS()
class CARAVAN_API ARobotAICharacter : public ABaseAICharacter
{
	GENERATED_UCLASS_BODY()

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<UInteractionComponent> InteractionComponent = nullptr;
};
