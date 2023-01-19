// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "CaravanCharacter.h"
#include "DrawDebugHelpers.h"

/*static */FInteractionChoice UInteractableComponent::s_MenuBackChoice = {TEXT("Back"), FText::FromString(TEXT("Back")), FText::GetEmpty(), EInteractionType::MenuBack};

UInteractableComponent::UInteractableComponent()
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ACaravanCharacter* Player = Cast< ACaravanCharacter >(TargetingPawn))
	{
		const float s_FontSizeObjectName = 2.f;
		const float s_FontSizeSubTitle = 1.1f;
		const float s_FontSizeChoiceDefault = 1.35f;
		const float s_FontSizeChoiceSelected = 1.5f;
		const float s_FontSizeChoiceDescription = 1.25f;
		const FColor s_ColorObjectName = FColor::Green;
		const FColor s_ColorSubTitle = FColor::Emerald;
		const FColor s_ColorChoiceDefault = FColor::White;
		const FColor s_ColorChoiceSelected = FColor::Cyan;
		const FColor s_ColorChoiceParent(25, 200, 200); // Close to Cyan
		const FColor s_ColorChoiceDescription = FColor::Silver;
		const float s_ObjectNameOffsetZ = 40.f;
		const float s_SubTitleOffsetZ = 44.f;
		const float s_ChoiceOffsetZ = 31.5f;
		const float s_ChoiceOffsetX = 24.f;
		const float s_ChoiceDescOffsetZ = 30.f;
		const float s_ChoiceDescOffsetX = 24.f;

		FVector InitialWorldLocation = GetComponentLocation();
		FVector TextOffsetDown = FVector::ZeroVector;
		FVector TextOffsetRight = FVector::ZeroVector;

		// Offset based on Camera orientation
		if (class UCameraComponent* Camera = Player->GetCameraComponent();
			IsValid(Camera))
		{
			const FVector CameraUp = Camera->GetUpVector();
			const FVector CameraRight = Camera->GetRightVector();
			const FVector CameraForward = Camera->GetForwardVector();
			InitialWorldLocation += (CameraRight * InteractionChoiceScreenOffset.X) + (CameraUp * InteractionChoiceScreenOffset.Y);

			TextOffsetDown = CameraUp * -1.f;
			TextOffsetRight = CameraRight;
		}

		FVector TextWorldLocation = InitialWorldLocation;

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
				TextWorldLocation += TextOffsetDown * s_ObjectNameOffsetZ;
			else
				TextWorldLocation += TextOffsetDown * s_SubTitleOffsetZ;
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

			TextWorldLocation += TextOffsetDown * s_SubTitleOffsetZ;
		}

		// Input choices
		TArray< FInteractionChoice > ChoicesToDraw;
		if (HasInteractionChoices())
		{
			ChoicesToDraw = InteractionChoices;
		}
		else
		{
			ChoicesToDraw.Add(PrimaryInteractionChoice);
		}

		if (!ParentChoiceID.IsNone())
		{
			DrawDebugString(
				GetWorld(),
				TextWorldLocation,
				ParentChoiceID.ToString(),
				nullptr,
				s_ColorChoiceParent,
				0.f,
				true,
				s_FontSizeChoiceSelected
			);

			TextWorldLocation += TextOffsetDown * s_ChoiceOffsetZ;
			TextWorldLocation += TextOffsetRight * s_ChoiceOffsetX;
		}

		const bool bHasMultipleChoices = ChoicesToDraw.Num() > 1;

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

			TextWorldLocation += TextOffsetDown * s_ChoiceOffsetZ;

			if (IsSelectedChoice &&
				!Choice.InteractionDescription.IsEmpty())
			{
				TextWorldLocation += TextOffsetRight * s_ChoiceDescOffsetX; // Indent

				const FString DescriptionString = Choice.InteractionDescription.ToString();

				TArray<FString> Lines;
				DescriptionString.ParseIntoArray(Lines, TEXT("\\n"), true);
				for (const FString& LineStr : Lines)
				{
					DrawDebugString(
						GetWorld(),
						TextWorldLocation,
						LineStr,
						nullptr,
						s_ColorChoiceDescription,
						0.f,
						true,
						s_FontSizeChoiceDescription
					);
					
					TextWorldLocation += TextOffsetDown * s_ChoiceDescOffsetZ; // Space between description lines
				}

				TextWorldLocation -= TextOffsetRight * s_ChoiceDescOffsetX; // Un-Indent
				//TextWorldLocation += TextOffsetDown * s_ChoiceDescOffsetZ; // One final vertical space before next choice
			}
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

FName UInteractableComponent::GetCurrentInteractionSubChoice() const
{
	return InteractionSubChoiceStack.IsEmpty() ? FName() : InteractionSubChoiceStack.Top();
}

void UInteractableComponent::RebuildInteractionChoices()
{
	OnRebuildInteractionChoices.Broadcast(InteractingPawn, this);

	// Add 'Back' interaction to sub-choice, if any
	if (!InteractionSubChoiceStack.IsEmpty() && 
		!InteractionChoices.IsEmpty() && 
		InteractionChoices.Top().InteractionID != s_MenuBackChoice.InteractionID)
	{
		InteractionChoices.Add(s_MenuBackChoice);
	}

	// Clamp index in case the choices have changed
	CurrentInteractionChoiceIndex = FMath::Min(CurrentInteractionChoiceIndex, InteractionChoices.Num() - 1);
}

void UInteractableComponent::SetTargeting(APawn* InTargetingPawn, bool bTargeting)
{
	if (bTargeting)
	{
		TargetingPawn = InTargetingPawn;

		// If using dynamic choices, rebuild before showing
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
		InteractionSubChoiceStack.Empty();
		ParentChoiceID = FName();
		TargetingPawn = NULL;

		OnInteractUntarget.Broadcast(NULL, this);
	}
}

bool UInteractableComponent::Interact(APawn* InInteractingPawn)
{
	InteractData data(InInteractingPawn);
	InteractSelect(data);

	const FInteractionChoice& Choice = GetCurrentInteractionChoice();
	OnInteract.Broadcast(InInteractingPawn, this, Choice);

	// Update sub-choices
	if (Choice.InteractionID == s_MenuBackChoice.InteractionID)
	{
		InteractionSubChoiceStack.Pop();
		ParentChoiceID = InteractionSubChoiceStack.IsEmpty() ? FName() : InteractionSubChoiceStack.Top();
	}
	else if (!Choice.SubChoiceInteractionID.IsNone())
	{
		InteractionSubChoiceStack.Add(Choice.SubChoiceInteractionID);
		ParentChoiceID = Choice.InteractionID;
	}

	// If using dynamic choices, rebuild after interaction, just in case the interaction has changed the choices
	if (bBuildInteractionChoicesDynamic)
	{
		RebuildInteractionChoices();
	}

	return true;
}

bool UInteractableComponent::IsTargeted() const
{
	return TargetingPawn != NULL;
}