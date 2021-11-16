// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "Caravan.h"
#include "CraftResourceActor.h"
#include "Engine.h"

UInteractable::UInteractable(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

ECraftResourceType IInteractable::GetResourceType() const 
{ 
	return ECraftResourceType::Invalid;
}

//void IInteractable::OnInteractFocus(const InteractData& interactData) {}
//void IInteractable::OnInteractUnFocus(const InteractData& interactData) {}
//EInteractionType IInteractable::OnInteractSelect(const InteractData& interactData) { return EInteractionType::None; }