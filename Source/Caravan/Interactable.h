// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Interactable.generated.h"

class APawn;
enum ECraftResourceType;

UENUM(BlueprintType)
enum EInteractionType
{
	None,

	ResourceMine,
	ResourceCollect,

	CaravanPull,
	CaravanToggleOpen,
	CaravanBuildAttachment,
};

UINTERFACE(MinimalAPI)
class UInteractable :public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInteractable
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FString GetInteractionName() const { return FString(""); }
	virtual ECraftResourceType GetResourceType() const;

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
	virtual void OnInteractFocus(const InteractData& interactData);
	// Called when actor loses focus on this object
	virtual void OnInteractUnFocus(const InteractData& interactData);
	// Called when actor interacts with the object, returns the type of interaction that occured
	virtual EInteractionType OnInteractSelect(const InteractData& interactData);
};