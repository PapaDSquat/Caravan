// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Interactable.generated.h"

enum class ECraftResourceType : uint8;

UINTERFACE(MinimalAPI)
class UInteractable :public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInteractable // TODO: Remove
{
	GENERATED_IINTERFACE_BODY()

public:
	//virtual FString GetInteractionName() const { return FString(""); }
	virtual ECraftResourceType GetResourceType() const;
};