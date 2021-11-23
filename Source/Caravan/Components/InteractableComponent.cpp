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

	PrimaryInteractionChoice.InteractionName = PrimaryInteractionName.ToString();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ACaravanCharacter* Player = Cast< ACaravanCharacter >(TargetingPawn))
	{
		static FVector2D s_ScreenOffset(50.f, 0.f);
		static float s_FontSize = 1.35f;
		static float s_ChoiceOffsetZ = 30.f;

		FVector WorldStartLocation = GetComponentLocation();

		// Offset based on Camera orientation
		class UCameraComponent* Camera = Player->GetFollowCamera();
		if (IsValid(Camera))
		{
			const FVector CameraUp = Camera->GetUpVector();
			const FVector CameraRight = Camera->GetRightVector();
			WorldStartLocation += (CameraRight * s_ScreenOffset.X) + (CameraUp * s_ScreenOffset.Y);
		}

		TArray< FInteractionChoice > ChoicesToDraw;
		if (HasInteractionChoices())
		{
			ChoicesToDraw = InteractionChoices;
		}
		else
		{
			ChoicesToDraw.Add(PrimaryInteractionChoice);
		}

		FVector WorldOffset = FVector::ZeroVector;

		for(int i=0; i<ChoicesToDraw.Num(); ++i)
		{
			const FInteractionChoice& Choice = ChoicesToDraw[i];
			const bool IsSelectedChoice = ChoicesToDraw.Num() == 1 || i == CurrentInteractionChoiceIndex;

			const FColor TextColor = IsSelectedChoice ? FColor::Cyan : FColor::White;

			DrawDebugString(
				GetWorld(),
				WorldStartLocation + WorldOffset,
				Choice.InteractionName,
				nullptr,
				TextColor,
				0.f,
				true,
				s_FontSize
			);

			WorldOffset.Z -= s_ChoiceOffsetZ;
		}
	}
}

void UInteractableComponent::SelectPrevChoice()
{
	if (!HasInteractionChoices())
		return;

	CurrentInteractionChoiceIndex = FMath::Max(CurrentInteractionChoiceIndex - 1, 0);
}

void UInteractableComponent::SelectNextChoice()
{
	if (!HasInteractionChoices())
		return;

	CurrentInteractionChoiceIndex = FMath::Min(CurrentInteractionChoiceIndex + 1, InteractionChoices.Num() - 1);
}

bool UInteractableComponent::HasInteractionChoices() const
{
	return InteractionChoices.Num() > 0;
}

const FInteractionChoice& UInteractableComponent::GetCurrentInteractionChoice() const
{
	if (CurrentInteractionChoiceIndex < 0)
		return PrimaryInteractionChoice;
	
	check(CurrentInteractionChoiceIndex < InteractionChoices.Num());
	return InteractionChoices[CurrentInteractionChoiceIndex];
}

void UInteractableComponent::SetInteractionChoices(const TArray< FInteractionChoice >& Choices)
{
	InteractionChoices = Choices;
}

void UInteractableComponent::SetTargeting(APawn* InTargetingPawn, bool bTargeting)
{
	if (bTargeting)
	{
		TargetingPawn = InTargetingPawn;
		if (HasInteractionChoices())
		{
			CurrentInteractionChoiceIndex = 0;
		}

		OnInteractTarget.Broadcast(TargetingPawn, this);
	}
	else
	{
		CurrentInteractionChoiceIndex = -1;
		TargetingPawn = NULL;
		OnInteractUntarget.Broadcast(NULL, this);
	}
}

bool UInteractableComponent::Interact(APawn* InInteractingPawn)
{
	InteractData data(InInteractingPawn);
	InteractSelect(data);

	// TODO
	const FInteractionChoice& Choice = GetCurrentInteractionChoice();
	OnInteract.Broadcast(InInteractingPawn, this, Choice);
	return true;
}

bool UInteractableComponent::IsTargeted() const
{
	return TargetingPawn != NULL;
}