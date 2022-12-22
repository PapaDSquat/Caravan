// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "CaravanCharacter.h"
#include "DrawDebugHelpers.h"

UInteractableComponent::UInteractableComponent()
	: PrimaryInteractionName( FText::FromString("UNNAMED_INTERACTION") )
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryInteractionChoice.InteractionName = PrimaryInteractionName;
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ACaravanCharacter* Player = Cast< ACaravanCharacter >(TargetingPawn))
	{
		static float s_FontSizeObjectName = 2.f;
		static float s_FontSizeSubTitle = 1.1f;
		static float s_FontSizeChoiceDefault = 1.35f;
		static float s_FontSizeChoiceSelected = 1.5f;
		static FColor s_ColorObjectName = FColor::Green;
		static FColor s_ColorSubTitle = FColor::Emerald;
		static FColor s_ColorChoiceDefault = FColor::White;
		static FColor s_ColorChoiceSelected = FColor::Cyan;
		static float s_ObjectNameOffsetZ = 40.f;
		static float s_SubTitleOffsetZ = 44.f;
		static float s_ChoiceOffsetZ = 31.5f;

		FVector InitialWorldLocation = GetComponentLocation();

		// Offset based on Camera orientation
		class UCameraComponent* Camera = Player->GetFollowCamera();
		if (IsValid(Camera))
		{
			const FVector CameraUp = Camera->GetUpVector();
			const FVector CameraRight = Camera->GetRightVector();
			const FVector CameraForward = Camera->GetForwardVector();
			InitialWorldLocation += (CameraRight * InteractionChoiceScreenOffset.X) + (CameraUp * InteractionChoiceScreenOffset.Y);
		}

		FVector TextWorldLocation = InitialWorldLocation;
		
		const bool bHasMultipleChoices = HasInteractionChoices();
		TArray< FInteractionChoice > ChoicesToDraw;
		if (bHasMultipleChoices)
		{
			ChoicesToDraw = InteractionChoices;
		}
		else
		{
			ChoicesToDraw.Add(PrimaryInteractionChoice);
		}

		const bool bHasTitle = !InteractableObjectName.IsEmpty();
		const bool bHasSubTitle = !InteractableObjectSubTitle.IsEmpty();

		if (bHasTitle)
		{
			DrawDebugString(
				GetWorld(),
				TextWorldLocation,
				InteractableObjectName.ToString(),
				nullptr,
				s_ColorObjectName,
				0.f,
				true,
				s_FontSizeObjectName
			);

			if(bHasSubTitle)
				TextWorldLocation.Z -= s_ObjectNameOffsetZ;
			else
				TextWorldLocation.Z -= s_SubTitleOffsetZ;
		}

		if (bHasSubTitle)
		{
			DrawDebugString(
				GetWorld(),
				TextWorldLocation,
				InteractableObjectSubTitle.ToString(),
				nullptr,
				s_ColorSubTitle,
				0.f,
				true,
				s_FontSizeSubTitle
			);

			TextWorldLocation.Z -= s_SubTitleOffsetZ;
		}

		for(int i=0; i<ChoicesToDraw.Num(); ++i)
		{
			const FInteractionChoice& Choice = ChoicesToDraw[i];
			const bool IsSelectedChoice = ChoicesToDraw.Num() == 1 || i == CurrentInteractionChoiceIndex;

			const FString TextString = FString::Format(TEXT("{0}{1}"), { IsSelectedChoice && bHasMultipleChoices ? TEXT(">") : TEXT(""), Choice.InteractionName.ToString() });
			const FColor TextColor = IsSelectedChoice ? s_ColorChoiceSelected : s_ColorChoiceDefault;
			const float TextSize = IsSelectedChoice ? s_FontSizeChoiceSelected : s_FontSizeChoiceDefault;

			DrawDebugString(
				GetWorld(),
				TextWorldLocation,
				TextString,
				nullptr,
				TextColor,
				0.f,
				true,
				TextSize
			);

			TextWorldLocation.Z -= s_ChoiceOffsetZ;
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

void UInteractableComponent::RebuildInteractionChoices()
{
	OnRebuildInteractionChoices.Broadcast(InteractingPawn, this);
}

void UInteractableComponent::SetTargeting(APawn* InTargetingPawn, bool bTargeting)
{
	if (bTargeting)
	{
		TargetingPawn = InTargetingPawn;

		if (bBuildInteractionChoicesDynamic)
		{
			RebuildInteractionChoices();
		}

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