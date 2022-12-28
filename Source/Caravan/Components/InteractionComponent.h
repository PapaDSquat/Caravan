#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class APawn;
class UInteractableComponent;

UCLASS(BlueprintType)
class CARAVAN_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact(ACharacter* InteractingCharacter, UInteractableComponent* Interactable);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetFocusedActor() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetTargetedActor() const;

	// Only true for the single frame that the interaction is triggered
	bool IsInteracting() const { return Interacting; }
	
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	TObjectPtr<UInteractableComponent> InteractTarget = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	TObjectPtr<UInteractableComponent> InteractFocus = NULL; // TODO Rename

	// Only true for the single frame that the interaction is triggered. Used for anim
	// TODO : Change to state flag
	bool Interacting{ false };
};
