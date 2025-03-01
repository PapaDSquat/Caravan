// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaravanCharacter.h"
#include "Caravan.h"
#include "CaravanActor.h"
#include "CaravanGameMode.h"
#include "Components/InteractableComponent.h"
#include "Components/InteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MultiToolActor.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"
#include "Utils/CaravanCollisionTypes.h"
#include "WorldBuilder/WorldBuilderSubsystem.h"

TAutoConsoleVariable CVar_InteractionTargetDebug(
	TEXT("Interaction.Target"),
	false,
	TEXT("Toggle target name for Player interaction"),

	ECVF_Cheat);

TAutoConsoleVariable CVar_InteractionOverlayDebug(
	TEXT("Interaction.Overlay"),
	false,
	TEXT("Toggle debug overlay for Player interaction"),
	ECVF_Cheat);

ACaravanCharacter::ACaravanCharacter(const class FObjectInitializer& ObjInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	// GetCharacterMovement()->bConstrainToPlane = true;
	// GetCharacterMovement()->bSnapToPlaneAtStart = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	AddOwnedComponent(InteractionComponent);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	AddOwnedComponent(InventoryComponent);

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	SetActorTickEnabled(true);
}

void ACaravanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn the MultiTool and attach to Weapon socket
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	MultiToolActor = GetWorld()->SpawnActor<AMultiToolActor>(SpawnParameters);
	if (MultiToolActor)
	{
		const USkeletalMeshSocket* pWeaponSocket = GetMesh()->GetSocketByName(FName("WeaponSocket"));
		if (pWeaponSocket)
		{
			pWeaponSocket->AttachActor(MultiToolActor, GetMesh());
		}
	}

	// Dwindle Setup
	DwindleState.TimeBeforeReset = LoiterTime;
	DwindleState.LastActorLocation = GetActorLocation();
	IsLoitering = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACaravanCharacter::SetupPlayerInputComponent(class UInputComponent* inputComponent)
{
	// Set up gameplay key bindings
	check(inputComponent);
	inputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	inputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	inputComponent->BindAxis("MoveForward", this, &ACaravanCharacter::MoveForward);
	inputComponent->BindAxis("MoveRight", this, &ACaravanCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	// inputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// inputComponent->BindAxis("TurnRate", this, &ACaravanCharacter::TurnAtRate);
	// inputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	// inputComponent->BindAxis("LookUpRate", this, &ACaravanCharacter::LookUpAtRate);

	// handle touch devices
	inputComponent->BindTouch(IE_Pressed, this, &ACaravanCharacter::TouchStarted);
	inputComponent->BindTouch(IE_Released, this, &ACaravanCharacter::TouchStopped);

	// Interact
	inputComponent->BindAction("Interact", IE_Pressed, this, &ACaravanCharacter::InteractSelect);
	inputComponent->BindAction("Target", IE_Pressed, this, &ACaravanCharacter::OnTargetActivate);
	inputComponent->BindAction("Target", IE_Released , this, &ACaravanCharacter::OnTargetDeactivate);
	inputComponent->BindAction("InteractChoiceScrollUp", IE_Pressed, this, &ACaravanCharacter::InteractChoiceScrollUp);
	inputComponent->BindAction("InteractChoiceScrollDown", IE_Pressed, this, &ACaravanCharacter::InteractChoiceScrollDown);

	// Debug
	inputComponent->BindAction("DEBUG_ResetResourceGrid", IE_Pressed, this, &ACaravanCharacter::DEBUG_ResetResourceGrid);
}

void ACaravanCharacter::DEBUG_ResetResourceGrid()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (UWorldBuilderSubsystem* WorldBuilderSubsystem = GameInstance->GetSubsystem<UWorldBuilderSubsystem>())
	{
		WorldBuilderSubsystem->ResetResourceGrid();
	}
}

void ACaravanCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ACaravanCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ACaravanCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACaravanCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACaravanCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACaravanCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACaravanCharacter::InteractSelect()
{
	if (IsCarryingCaravan())
	{
		// Cancel
		FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, false);
		ActiveCaravan->DetachFromActor(rules);
		ActiveCaravan = NULL;
	}
	else if (IsValid(InteractFocus))
	{
		InteractionComponent->Interact(this, InteractFocus);
		/*
		const EInteractionType interactionType = InteractFocus->InteractSelect(interactData);

		switch (interactionType)
		{
		case EInteractionType::CaravanPull:
			if (ACaravanActor* pCaravan = Cast<ACaravanActor>(focusedActor))
			{
				this->SetActorLocation(pCaravan->GetCarrySocketLocation());
				this->SetActorRotation(pCaravan->GetCarrySocketRotation());

				FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, false);
				pCaravan->AttachToActor(this, rules, FName("PullSocket"));

				ActiveCaravan = pCaravan;
			}
			break;
		}
		
		*/
		AActor* focusedActor = GetFocusedActor();
		if (focusedActor && focusedActor->IsPendingKillPending())
		{
			// It's going to be destroyed, so cancel targetting if any
			if (IsTargeting)
			{
				OnTargetDeactivate();
			}
		}
	}
}

void ACaravanCharacter::InteractChoiceScrollUp()
{
	if (IsTargeting && InteractTarget)
	{
		InteractTarget->SelectPrevChoice();
	}
}

void ACaravanCharacter::InteractChoiceScrollDown()
{
	if (IsTargeting && InteractTarget)
	{
		InteractTarget->SelectNextChoice();
	}
}

void ACaravanCharacter::OnTargetActivate()
{
	if (!IsTargeting) // Wait for Deactivate
	{
		InteractTarget = InteractFocus;
		//InteractTarget->GlobalDestroyPhysicsDelegate.Add(this, &ACaravanActor::OnInteractComponentDeactivated);

		IsTargeting = (InteractTarget != NULL);
		if (IsTargeting)
		{
			InteractTarget->SetTargeting(this, true);

			IsTargetingWithChoices = InteractTarget->HasInteractionChoices();
		}
	}
}

void ACaravanCharacter::OnTargetDeactivate()
{
	if (InteractTarget)
	{
		InteractTarget->SetTargeting(NULL, false);
	}
	InteractTarget = NULL;
	IsTargeting = IsTargetingWithChoices = false;
}

void ACaravanCharacter::SetIsInCaravanCamp(bool bInCamp)
{
	ERobotAILocale NewLocale;
	if (bInCamp)
	{
		NewLocale = ERobotAILocale::CaravanCamp;
	}
	else
	{
		NewLocale = ERobotAILocale::PlayerExpedition;
	}

	if (WorldLocale != NewLocale)
	{
		WorldLocale = NewLocale;
		OnLocaleChangeEvent.Broadcast(this, WorldLocale);
	}
}

void ACaravanCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateDwindleState(DeltaSeconds);

	if (IsCarryingCaravan())
	{
		// Continue in interacting state for now
		// Cancel other interaction until next click where we release the caravan
		return;
	}

	// Check if in player camp or on expedition
	if (ACaravanActor* CaravanActor = GetCaravanGameMode()->CaravanActor)
	{
		const float DistanceToCaravan = (CaravanActor->GetActorLocation() - this->GetActorLocation()).Size();
		SetIsInCaravanCamp(DistanceToCaravan < ExpeditionRange);
	}

	if (IsTargeting && IsValid(InteractTarget))
	{
		// Orient towards the target
		const FVector targetLocation = InteractTarget->GetComponentLocation();
		const FVector offsetFromTarget = (targetLocation - GetActorLocation());
		
		// Build new rotation from distance vector
		FRotator newRotation = offsetFromTarget.GetSafeNormal().Rotation();
		newRotation.Pitch = GetActorRotation().Pitch; // Use the current pitch
		SetActorRotation(newRotation);

		//FVector targetOrigin, targetExtent;
		//targetActor->GetActorBounds(false, targetOrigin, targetExtent);
		//TargetBaseLocation = targetOrigin + FVector(0.f, 0.f, -(targetExtent.Z * 0.75f)); // HACK offset

		TargetBaseLocation = targetLocation;

		if (CVar_InteractionTargetDebug->GetBool())
		{
			GEngine->AddOnScreenDebugMessage(0, -1.f, FColor::Green, InteractTarget->GetCurrentInteractionChoice().InteractionName.ToString());
		}

		if (CVar_InteractionOverlayDebug->GetBool())
		{
			const AActor* targetActor = GetTargetedActor();
			FVector targetOrigin, targetExtent;
			targetActor->GetActorBounds(false, targetOrigin, targetExtent);

			DrawDebugBox(
				GetWorld(),
				targetOrigin,
				targetExtent,
				FColor(255, 0, 255), // Pink
				false, 0.1f
				);

 			DrawDebugSphere(
 				GetWorld(),
 				targetOrigin,
 				25.f,
 				16,
 				FColor(255, 0, 255),// Pink
 				false, 0.1
 				);

			DrawDebugSphere(
				GetWorld(),
				TargetBaseLocation,
				75.f,
				16,
				FColor(0, 0, 255),// Blue
				false, 0.1
				);
			
			DrawDebugLine(
				GetWorld(),
				GetActorLocation(),
				TargetBaseLocation,
				FColor(0, 0, 255),// Blue
				false, 0.1, 0,
				6.f
				);
		}

		// Skip raycasts until targeting is canceled
		return;
	}

	// TODO: potential cone trace
	// Raycast from player's position along its forward vector
	TArray<SInteractTraceData> traceDataList;

	const FVector StartTrace = GetActorLocation();
	const FVector Direction = GetActorRotation().Vector();

	//TODO: Improve
	//static float VERTICAL_INTERVAL = 15.f;
	//static float HORIZONTAL_INTERVAL = 15.f;
	
	// Perform foward to down traces first, and only do the rest if theres no hits

	SInteractTraceResult traceResult;
	bool traceSuccess = false;

	FVector EndTrace = StartTrace + (Direction * InteractLength);
	traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

	// Down
	EndTrace = StartTrace + (Direction.RotateAngleAxis(15.f, GetActorRightVector()) * InteractLength);
	traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

	EndTrace = StartTrace + (Direction.RotateAngleAxis(30.f, GetActorRightVector()) * InteractLength);
	traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

	EndTrace = StartTrace + (Direction.RotateAngleAxis(45.f, GetActorRightVector()) * InteractLength);
	traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

	EndTrace = StartTrace + (Direction.RotateAngleAxis(60.f, GetActorRightVector()) * InteractLength);
	traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

	traceSuccess = TryInteractTrace(traceDataList, traceResult);
	
	// If it fails, try the other sides
	if (!traceSuccess)
	{
		traceDataList.Empty();

		// Left
		EndTrace = StartTrace + (Direction.RotateAngleAxis(-15.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		EndTrace = StartTrace + (Direction.RotateAngleAxis(-30.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		// Left Diagonal
		EndTrace = StartTrace + (Direction.RotateAngleAxis(15.f, GetActorRightVector()).RotateAngleAxis(-15.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		EndTrace = StartTrace + (Direction.RotateAngleAxis(30.f, GetActorRightVector()).RotateAngleAxis(-30.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		// Right
		EndTrace = StartTrace + (Direction.RotateAngleAxis(15.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		EndTrace = StartTrace + (Direction.RotateAngleAxis(30.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		// Right Diagonal
		EndTrace = StartTrace + (Direction.RotateAngleAxis(15.f, GetActorRightVector()).RotateAngleAxis(15.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		EndTrace = StartTrace + (Direction.RotateAngleAxis(30.f, GetActorRightVector()).RotateAngleAxis(30.f, GetActorUpVector()) * InteractLength);
		traceDataList.Push(SInteractTraceData(StartTrace, EndTrace));

		traceSuccess = TryInteractTrace(traceDataList, traceResult);
	}

	if (traceSuccess)
	{
		LastTraceResult = traceResult;

		UInteractableComponent::InteractData interactData(this, LastTraceResult.HitResult);
		// Focus on new one if it's not the same as previous
		if (LastTraceResult.HitComponent != InteractFocus)
		{
			// Unfocus previous
			if (IsValid(InteractFocus))
			{
				InteractFocus->InteractUnFocus(interactData);
			}

			InteractFocus = LastTraceResult.HitComponent;
		}

		InteractFocus->InteractFocus(interactData);
	}
	else
	{
		// TODO: Add delay before lose focus
		if (IsValid(InteractFocus))
		{
			UInteractableComponent::InteractData interactData(this);
			InteractFocus->InteractUnFocus(interactData);
			InteractFocus = NULL;

			// Cancel targetting
			if (IsTargeting)
			{
				OnTargetDeactivate();
			}
		}
	}
}

void ACaravanCharacter::UpdateDwindleState(float DeltaSeconds)
{
	DwindleState.TimeBeforeReset -= DeltaSeconds;

	const FVector currentActorLocation = GetActorLocation();
	const float distance = FVector::Distance(currentActorLocation, DwindleState.LastActorLocation);

	// Check dwindle at every interval
	if (DwindleState.TimeBeforeReset <= 0.f)
	{
		IsLoitering = (distance <= LoiterRange);

		DwindleState.TimeBeforeReset = LoiterTime;
		DwindleState.LastActorLocation = GetActorLocation();
	}

	// Check that we are not already outside of dwindle range
	if (!IsLoitering && distance > LoiterRange)
	{
		IsLoitering = false;
	}
}

bool ACaravanCharacter::TryInteractTrace(const TArray<SInteractTraceData>& TraceDataList, SInteractTraceResult& OutTraceResult)
{
	int32 ClosestIndex = 0;
	FHitResult ClosesHitResult;
	UInteractableComponent* ClosestInteractable = NULL;
	float ClosestInteractableDistance = TNumericLimits<float>::Max(); 

	for (int32 traceIdx = 0; traceIdx < TraceDataList.Num(); ++traceIdx)
	{
		const SInteractTraceData& TraceData = TraceDataList[traceIdx];

		FCollisionQueryParams TraceParams(FName(*FString::Printf(TEXT("PlayerInteractTrace_%d"), traceIdx)), true, this);
		// TraceParams.bTraceAsyncScene = true; // #TODO-Version-Migration
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult hitResult(ForceInit);
		GetWorld()->LineTraceSingleByChannel(hitResult, TraceData.Start, TraceData.End, CARAVAN_OBJECT_CHANNEL_INTERACTABLE, TraceParams);

		const AActor* HitActor = hitResult.GetActor();

		if (CVar_InteractionOverlayDebug.GetValueOnGameThread() == true)
		{
			FColor debugColor = !IsValid(HitActor)
				? FColor(255, 0, 0)  // Red for miss
				: FColor(0, 255, 0); // Green for hit

			DrawDebugLine(
				GetWorld(),
				TraceData.Start,
				TraceData.End,
				debugColor,
				false, 0.1, 0,
				7.5f
			);
		}

		if (!HitActor)
			continue;

		TArray<UInteractableComponent*> InteractableComponents;
		HitActor->GetComponents<UInteractableComponent>(InteractableComponents);
		if (InteractableComponents.Num() == 0)
			continue;

		for (UInteractableComponent* Comp : InteractableComponents)
		{
			if (!Comp->IsActive())
				continue;

			const FVector CompLocation = Comp->GetComponentLocation();
			const float distance = (CompLocation - hitResult.Location).Size();
			if (distance < ClosestInteractableDistance)
			{
				ClosestInteractableDistance = distance;
				ClosestInteractable = Comp;
				ClosesHitResult = hitResult;
				ClosestIndex = traceIdx;
			}
		}
	}

	if (CVar_InteractionOverlayDebug->GetBool())
	{
		if (IsValid(ClosestInteractable))
		{
			// GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Emerald, FString::Printf(TEXT("Location: %f, %f, %f"), ClosestInteractable->GetComponentLocation().X, ClosestInteractable->GetComponentLocation().Y, ClosestInteractable->GetComponentLocation().Z));
		}

		// Slightly bigger blue line for closest result
		DrawDebugLine(
			GetWorld(),
			TraceDataList[ClosestIndex].Start,
			TraceDataList[ClosestIndex].End,
			FColor(0, 0, 255), // Blue
			false, 0.1, 0,
			10.0f
			);
	}

	OutTraceResult.HitResult = ClosesHitResult;
	OutTraceResult.HitComponent = ClosestInteractable;
	OutTraceResult.HitDistance = ClosestInteractableDistance;

	return (ClosestInteractable != NULL);
}

AActor* ACaravanCharacter::GetFocusedActor() const
{
	return IsValid(InteractFocus) ? InteractFocus->GetOwner() : NULL;
}

AActor* ACaravanCharacter::GetTargetedActor() const
{
	return IsValid(InteractTarget) ? InteractTarget->GetOwner() : NULL;
}

UInteractableComponent* ACaravanCharacter::GetFocusedInteractable() const
{
	return InteractFocus;
}

UInteractableComponent* ACaravanCharacter::GetTargetedInteractable() const
{
	return InteractTarget;
}

ACaravanGameMode* ACaravanCharacter::GetCaravanGameMode() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast< ACaravanGameMode >(World->GetAuthGameMode());
	}
	return nullptr;
}