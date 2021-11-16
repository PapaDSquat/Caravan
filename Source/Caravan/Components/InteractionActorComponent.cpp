// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractionActorComponent.h"
#include "Components/InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "CaravanCharacter.h"
#include "DrawDebugHelpers.h"

UInteractionActorComponent::UInteractionActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionActorComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

AActor* UInteractionActorComponent::GetFocusedActor() const
{
	return IsValid(InteractFocus) ? InteractFocus->GetOwner() : NULL;
}

AActor* UInteractionActorComponent::GetTargetedActor() const
{
	return IsValid(InteractTarget) ? InteractTarget->GetOwner() : NULL;
}