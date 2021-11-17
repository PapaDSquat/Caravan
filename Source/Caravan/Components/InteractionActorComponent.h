// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionActorComponent.generated.h"

class APawn;
class UInteractableComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARAVAN_API UInteractionActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionActorComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetFocusedActor() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetTargetedActor() const;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Has Target?
	//UFUNCTION(BlueprintCallable, Category = "Interaction")
	//bool IsTargeted() const;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	bool Interacting{ false }; // TODO Rename, add function

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	UInteractableComponent* InteractTarget = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	FVector TargetBaseLocation;

	// Interaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	UInteractableComponent* InteractFocus = NULL; // TODO Rename
};
