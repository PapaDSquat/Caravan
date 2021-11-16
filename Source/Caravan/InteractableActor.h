// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableActor.generated.h"

#define CARAVAN_OBJECT_CHANNEL_INTERACTABLE ECC_GameTraceChannel1

UCLASS()
class AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableActor();
	~AInteractableActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void Interact(APawn* interactingPawn);

	// IInteractable
	//virtual void OnInteractFocus(const InteractData& interactData) override;
	//virtual void OnInteractUnFocus(const InteractData& interactData) override;
	//virtual EInteractionType OnInteractSelect(const InteractData& interactData) override;

	UFUNCTION(BlueprintImplementableEvent, Category = Interaction)
	void NotifyOnInteractFirstFocus();

	UFUNCTION(BlueprintImplementableEvent, Category = Interaction)
	void NotifyOnInteractUnFocus();

	UFUNCTION(BlueprintImplementableEvent, Category = Interaction)
	void NotifyOnInteractSelect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	float MinimumInteractDistance = 500.f;

	UPROPERTY(BlueprintReadOnly, Category = Interaction)
	bool IsFocused = false;
};
