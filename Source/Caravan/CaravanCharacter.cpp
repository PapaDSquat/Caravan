// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaravanCharacter.h"
#include "Caravan.h"
#include "CaravanActor.h"
#include "Components/InteractableComponent.h"
#include "Debug/CaravanConsoleVariables.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "InteractableActor.h"
#include "MultiToolActor.h"
#include "RPG/InventoryComponent.h"
#include "WorldBuilder/WorldBuilderSubsystem.h"

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Inventory = FindComponentByClass< UInventoryComponent >();
	if (Inventory == NULL)
	{
		Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	}

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
	inputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	inputComponent->BindAxis("TurnRate", this, &ACaravanCharacter::TurnAtRate);
	inputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	inputComponent->BindAxis("LookUpRate", this, &ACaravanCharacter::LookUpAtRate);

	// handle touch devices
	inputComponent->BindTouch(IE_Pressed, this, &ACaravanCharacter::TouchStarted);
	inputComponent->BindTouch(IE_Released, this, &ACaravanCharacter::TouchStopped);

	// Custom actions
	inputComponent->BindAction("Interact", IE_Pressed, this, &ACaravanCharacter::Interact);
	inputComponent->BindAction("Target", IE_Pressed, this, &ACaravanCharacter::OnTargetActivate);
	inputComponent->BindAction("Target", IE_Released , this, &ACaravanCharacter::OnTargetDeactivate);

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

void ACaravanCharacter::Interact()
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
		AActor* focusedActor = GetFocusedActor();
		const UInteractableComponent::InteractData interactData(this, LastTraceResult.HitResult);
		InteractFocus->Interact(this);
		/*
		const EInteractionType interactionType = InteractFocus->InteractSelect(interactData);
		Interacting = true; // Reset on next tick

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
		if (focusedActor->IsPendingKillPending())
		{
			// It's going to be destroyed, so cancel targetting if any
			if (IsTargeting)
			{
				OnTargetDeactivate();
			}
		}
	}
}

void ACaravanCharacter::OnTargetActivate()
{
	if (!IsTargeting) // Wait for Deactivate
	{
		InteractTarget = InteractFocus;
		IsTargeting = (InteractTarget != NULL);
		if (IsTargeting)
		{
		}
	}
}

void ACaravanCharacter::OnTargetDeactivate()
{
	InteractTarget = NULL;
	IsTargeting = false;
}

void ACaravanCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateDwindleState(DeltaSeconds);

	Interacting = false;

	if (IsCarryingCaravan())
	{
		// Continue in interacting state for now
		// Cancel other interaction until next click where we release the caravan
		return;
	}

	// Orient towards the target
	if (IsTargeting && IsValid(InteractTarget))
	{
		AActor* targetActor = GetTargetedActor();

		// Build new rotation from distance vector
		FRotator newRotation = (targetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation();
		// Use the current pitch
		newRotation.Pitch = GetActorRotation().Pitch;
		this->SetActorRotation(newRotation);

		FVector targetLocation = targetActor->GetActorLocation();
		FVector targetOrigin, targetExtent;
		targetActor->GetActorBounds(false, targetOrigin, targetExtent);
		TargetBaseLocation = targetOrigin + FVector(0.f, 0.f, -(targetExtent.Z * 0.75f)); // HACK offset

		if (CVarPlayerDebug_ShowInteractionTarget.GetValueOnGameThread() == true)
		{
			GEngine->AddOnScreenDebugMessage(0, -1.f, FColor::Green, InteractTarget->InteractionName.ToString());
		}

		if (CVarPlayerDebug_ShowInteractionOverlay.GetValueOnGameThread() == true)
		{
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
 				75.f,
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

bool ACaravanCharacter::TryInteractTrace(const TArray<SInteractTraceData>& traceDataList, SInteractTraceResult& outTraceResult)
{
	int32 closestIndex = 0;
	FHitResult closesHitResult;
	UInteractableComponent* closestInteractable = NULL;
	float closestActorDistance = 9999999.f; // TODO: Find a define

	int32 traceTotal = traceDataList.Num();
	for (int32 traceIdx = 0; traceIdx < traceTotal; ++traceIdx)
	{
		const SInteractTraceData& traceData = traceDataList[traceIdx];

		FCollisionQueryParams TraceParams(FName(*FString::Printf(TEXT("PlayerInteractTrace_%d"), traceIdx)), true, this);
		// TraceParams.bTraceAsyncScene = true; // #TODO-Version-Migration
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult hitResult(ForceInit);
		GetWorld()->LineTraceSingleByChannel(hitResult, traceData.Start, traceData.End, CARAVAN_OBJECT_CHANNEL_INTERACTABLE, TraceParams);

		UInteractableComponent* hitComponent = nullptr;
		if (AActor* hitActor = hitResult.GetActor())
		{
			hitComponent = hitActor->FindComponentByClass<UInteractableComponent>();
			if (IsValid(hitComponent))
			{
				const FVector actorLocation = IsValid(hitComponent) ? hitComponent->GetOwner()->GetActorLocation() : hitActor->GetActorLocation();
				float distance = (actorLocation - this->GetActorLocation()).Size();
				if (distance < closestActorDistance) //&& // TODO: Fix
					//distance < interactableHitActor->MinimumInteractDistance) // Within interact range of this actor
				{
					distance = closestActorDistance;
					closestInteractable = hitComponent;
					closesHitResult = hitResult;
					closestIndex = traceIdx;
				}
			}
		}

		if (CVarPlayerDebug_ShowInteractionOverlay.GetValueOnGameThread() == true)
		{
			FColor debugColor = !IsValid(hitComponent)
				? FColor(255, 0, 0)  // Red for miss
				: FColor(0, 255, 0); // Green for hit

			DrawDebugLine(
				GetWorld(),
				traceData.Start,
				traceData.End,
				debugColor,
				false, 0.1, 0,
				7.5f
				);
		}
	}

	if (CVarPlayerDebug_ShowInteractionOverlay.GetValueOnGameThread() == true)
	{
		// Slightly bigger blue line for closest result
		DrawDebugLine(
			GetWorld(),
			traceDataList[closestIndex].Start,
			traceDataList[closestIndex].End,
			FColor(0, 0, 255), // Blue
			false, 0.1, 0,
			10.0f
			);
	}

	outTraceResult.HitResult = closesHitResult;
	outTraceResult.HitComponent = closestInteractable;
	outTraceResult.HitDistance = closestActorDistance;

	return (closestInteractable != NULL);
}

AActor* ACaravanCharacter::GetFocusedActor() const
{
	return IsValid(InteractFocus) ? InteractFocus->GetOwner() : NULL;
}

AActor* ACaravanCharacter::GetTargetedActor() const
{
	return IsValid(InteractTarget) ? InteractTarget->GetOwner() : NULL;
}