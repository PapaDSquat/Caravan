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

	MenuBack,
};

USTRUCT(BlueprintType)
struct FInteractionChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteractionID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionType InteractionType = EInteractionType::None;

	// Optional : interacting with this choice will trigger a sub-menu choice
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SubChoiceInteractionID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractTargetEvent, APawn*, InteractingPawn, class UInteractableComponent*, InteractableComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractEvent, APawn*, InteractingPawn, class UInteractableComponent*, InteractableComponent, const FInteractionChoice&, Choice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractRebuildChoicesEvent, APawn*, InteractingPawn, class UInteractableComponent*, InteractableComponent);

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
	FName GetCurrentInteractionSubChoice() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RebuildInteractionChoices();

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

	UPROPERTY(BlueprintAssignable)
	FInteractRebuildChoicesEvent OnRebuildInteractionChoices;

	//==================================================
	// Config Properties
	// Optional : Name of the Interactable Object itself, displayed in Interaction UI
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText InteractableObjectName;

	// Optional : Sub-title displayed in Interaction UI
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText InteractableObjectSubTitle;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FInteractionChoice PrimaryInteractionChoice;

	// Choices displayed after Player interacts with this interactable object
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TArray< FInteractionChoice > InteractionChoices;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FVector2D InteractionChoiceScreenOffset = FVector2D(50.f, 100.f);

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bBuildInteractionChoicesDynamic = false;
	//==================================================

private:
	// TODO : All this should be in InteractionComponent
	int CurrentInteractionChoiceIndex = -1;

	// TODO: kinda sucks cuz im assuming ID is display text
	TArray<FName> InteractionSubChoiceStack;
	FName ParentChoiceID;

	static FInteractionChoice s_MenuBackChoice;

	UPROPERTY(Transient)
	TObjectPtr<APawn> TargetingPawn;

	UPROPERTY(Transient)
	TObjectPtr<APawn> InteractingPawn;
};
