// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacterSpec.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

enum class ERobotAILocale : uint8;
class UInventoryComponent;
class UInteractableComponent;
struct FInteractionChoice;

UCLASS()
class CARAVAN_API ARobotAIController : public AAIController
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Caravan Robot")
	ARobotAICharacter* GetRobotOwner() const;

	UFUNCTION(BlueprintCallable, Category = "Caravan Robot")
	FName GetRobotName() const;

	UFUNCTION(BlueprintCallable, Category = "Caravan AI")
	void SetCurrentAILocale(ERobotAILocale newLocale);


	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FRobotAIProfile CharacterProfile;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	ERobotAILocale CurrentAILocale;

	UPROPERTY(BlueprintReadOnly, Category = "RPG")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly, Category = "RPG")
	UInteractableComponent* InteractableComponent;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
};
