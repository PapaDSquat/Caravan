// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractableComponent.generated.h"

class APawn;

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,

	ResourceMine,
	ResourceCollect,

	CaravanPull,
	CaravanToggleOpen,
	CaravanBuildAttachment,
};

USTRUCT(BlueprintType)
struct FInteractionChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName InteractionID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString InteractionName; // TODO: Change to FText InteractionText

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInteractionType InteractionType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractTargetEvent, APawn*, InteractingPawn, class UInteractableComponent*, InteractableComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractEvent, APawn*, InteractingPawn, class UInteractableComponent*, InteractableComponent, const FInteractionChoice&, Choice);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CARAVAN_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	struct InteractData
	{
		InteractData(APawn* pawn)
			: Pawn(pawn)
		{}
		InteractData(APawn* pawn, FHitResult traceResult)
			: Pawn(pawn)
			, TraceResult(traceResult)
		{}

		APawn* Pawn; // TODO: change to controller
		FHitResult TraceResult;
	};

	// Called when actor focuses on this object
	void InteractFocus(const InteractData& interactData) {}
	// Called when actor loses focus on this object
	void InteractUnFocus(const InteractData& interactData) {}
	// Called when actor interacts with the object, returns the type of interaction that occured
	EInteractionType InteractSelect(const InteractData& interactData) { return EInteractionType::None; }

	const FInteractionChoice& GetCurrentInteractionChoice() const;
	void SelectPrevChoice();
	void SelectNextChoice();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool HasInteractionChoices() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionChoices(const TArray< FInteractionChoice >& Choices);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetTargeting(APawn* InTargetingPawn, bool bTargeting);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact(APawn* InInteractingPawn);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsTargeted() const;
	
	UPROPERTY(BlueprintAssignable)
	FInteractTargetEvent OnInteractTarget;

	UPROPERTY(BlueprintAssignable)
	FInteractTargetEvent OnInteractUntarget;

	UPROPERTY(BlueprintAssignable)
	FInteractEvent OnInteract;

	//==================================================
	// Config Properties
	// Interaction name displayed when targeting the interactable (before opening interaction menu)
	UPROPERTY(EditAnywhere, Category="Interaction")
	FText PrimaryInteractionName;

	// Choices displayed after Player interacts with this interactable object
	UPROPERTY(EditAnywhere, Category = "Interaction", meta=(EditCondition="!bBuildInteractionChoicesInScripts"))
	TArray< FInteractionChoice > InteractionChoices;
	//==================================================

private:
	FInteractionChoice PrimaryInteractionChoice;
	int CurrentInteractionChoiceIndex = -1;

	APawn* TargetingPawn;
	APawn* InteractingPawn;
};
