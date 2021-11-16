// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "CaravanCharacter.h"
#include "DrawDebugHelpers.h"

UInteractableComponent::UInteractableComponent()
	: PrimaryInteractionName( FText::FromString("UNNAMED_INTERACTION") )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetingPawn.IsValid())
	{
		if (ACaravanCharacter* Player = Cast< ACaravanCharacter >(TargetingPawn.Pin().Get()))
		{
			class UCameraComponent* Camera = Player->GetFollowCamera();
			if (IsValid(Camera))
			{
				static FVector2D s_ScreenOffset(50.f, 0.f);
				const FVector CameraUp = Camera->GetUpVector();
				const FVector CameraRight = Camera->GetRightVector();

				const FVector WorldOffset = (CameraRight * s_ScreenOffset.X) + (CameraUp * s_ScreenOffset.Y);

				DrawDebugString(
					GetWorld(),
					WorldOffset,
					PrimaryInteractionName.ToString(),
					GetOwner(),
					FColor::Green,
					0.f
				);
			}
		}
		
	}
}

void UInteractableComponent::SetTargeting(APawn* InTargetingPawn, bool bTargeting)
{
	if (bTargeting)
	{
		TargetingPawn = TargetingPawn;
		OnInteractTarget.Broadcast(InTargetingPawn, this);
	}
	else
	{
		TargetingPawn = NULL;
		OnInteractUntarget.Broadcast(InTargetingPawn, this);
	}
}

bool UInteractableComponent::Interact(APawn* InInteractingPawn)
{
	InteractData data(InInteractingPawn);
	InteractSelect(data);

	// TODO
	FInteractionChoice Choice;
	//Choice.InteractionType
	//Choice.InteractionName
	OnInteract.Broadcast(InInteractingPawn, this, Choice);
	return true;
}

bool UInteractableComponent::IsTargeted() const
{
	return TargetingPawn.IsValid();
}