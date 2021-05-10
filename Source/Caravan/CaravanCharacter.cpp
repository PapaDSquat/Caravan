// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CaravanCharacter.h"
#include "Caravan.h"
#include "InteractableActor.h"
#include "CaravanActor.h"
#include "MultiToolActor.h"
#include "DrawDebugHelpers.h"
#include "WorldGenerator.h"
#include "Engine/SkeletalMeshSocket.h"

#define COLLISION_INTERACTABLE ECC_GameTraceChannel1

//////////////////////////////////////////////////////////////////////////
// ACaravanCharacter

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
	if (WorldGenerator)
	{
		inputComponent->BindAction("DEBUG_ResetResourceGrid", IE_Pressed, WorldGenerator, &AWorldGenerator::ResetResourceGrid);
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
	else if (InteractFocus != NULL)
	{
		// Do this before OnInteract since it will be destroyed
		if (ACraftResourceActor* pCraftResource = Cast<ACraftResourceActor>(InteractFocus))
		{
			ECraftResourceType resourceType = pCraftResource->GetResourceType();
			if (CraftResourceHelpers::IsValidType(resourceType))
			{
				CraftResourceCount[(int)resourceType] = CraftResourceCount[(int)resourceType] + 1;
			}
		}

		IInteractable::InteractData interactData(this, LastTraceResult.HitResult);
		EInteractionType interactionType = InteractFocus->OnInteractSelect(interactData);
		Interacting = true; // Reset on next tick

		switch (interactionType)
		{
		case EInteractionType::CaravanPull:
			if (ACaravanActor* pCaravan = Cast<ACaravanActor>(InteractFocus))
			{
				this->SetActorLocation(pCaravan->GetCarrySocketLocation());
				this->SetActorRotation(pCaravan->GetCarrySocketRotation());

				FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, false);
				pCaravan->AttachToActor(this, rules, FName("PullSocket"));

				ActiveCaravan = pCaravan;
			}
			break;
		}
		

		if (InteractFocus->IsPendingKillPending())
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
		TargetActor = InteractFocus;
		IsTargeting = (TargetActor != NULL);
		if (IsTargeting)
		{
		}
	}
}

void ACaravanCharacter::OnTargetDeactivate()
{
	TargetActor = NULL;
	IsTargeting = false;
}

void ACaravanCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Interacting = false;

	if (IsCarryingCaravan())
	{
		// Continue in interacting state for now
		// Cancel other interaction until next click where we release the caravan
		return;
	}

	// Orient towards the target
	if (IsTargeting && TargetActor != NULL)
	{
		// Build new rotation from distance vector
		FRotator newRotation = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation();
		// Use the current pitch
		newRotation.Pitch = GetActorRotation().Pitch;
		this->SetActorRotation(newRotation);

		FVector targetLocation = TargetActor->GetActorLocation();
		FVector targetOrigin, targetExtent;
		TargetActor->GetActorBounds(false, targetOrigin, targetExtent);
		TargetBaseLocation = targetOrigin + FVector(0.f, 0.f, -(targetExtent.Z * 0.75f)); // HACK offset

		if (DEBUG_ENABLED)
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

		IInteractable::InteractData interactData(this, LastTraceResult.HitResult);
		// Focus on new one if it's not the same as previous
		if (LastTraceResult.HitActor != InteractFocus)
		{
			// Unfocus previous
			if (InteractFocus)
			{
				InteractFocus->OnInteractUnFocus(interactData);
			}

			InteractFocus = LastTraceResult.HitActor;
		}

		InteractFocus->OnInteractFocus(interactData);
	}
	else
	{
		// TODO: Add delay before lose focus
		if (InteractFocus)
		{
			IInteractable::InteractData interactData(this);
			InteractFocus->OnInteractUnFocus(interactData);
			InteractFocus = NULL;

			// Cancel targetting
			if (IsTargeting)
			{
				OnTargetDeactivate();
			}
		}
	}
}

bool ACaravanCharacter::TryInteractTrace(const TArray<SInteractTraceData>& traceDataList, SInteractTraceResult& outTraceResult)
{
	int32 closestIndex = 0;
	FHitResult closesHitResult;
	AInteractableActor* closestInteractable = NULL;
	float closestActorDistance = 9999999.f; // TODO: Find a define

	int32 traceTotal = traceDataList.Num();
	for (int32 traceIdx = 0; traceIdx < traceTotal; ++traceIdx)
	{
		const SInteractTraceData& traceData = traceDataList[traceIdx];

		FCollisionQueryParams TraceParams(FName(*FString::Printf(TEXT("PlayerInteractTrace_%d"), traceIdx)), true, this);
		// TraceParams.bTraceAsyncScene = true; // #TODO-Version-Migration
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult hitResult(ForceInit);
		GetWorld()->LineTraceSingleByChannel(hitResult, traceData.Start, traceData.End, COLLISION_INTERACTABLE, TraceParams);

		
		AInteractableActor* interactableHitActor = nullptr;
		if (AActor* hitActor = hitResult.GetActor())
		{
			interactableHitActor = Cast<AInteractableActor>(hitActor);
			if (interactableHitActor)
			{
				float distance = (hitActor->GetActorLocation() - this->GetActorLocation()).Size();
				if (distance < closestActorDistance) //&& // TODO: Fix
					//distance < interactableHitActor->MinimumInteractDistance) // Within interact range of this actor
				{
					distance = closestActorDistance;
					closestInteractable = interactableHitActor;
					closesHitResult = hitResult;
					closestIndex = traceIdx;
				}
			}
		}

		if (DEBUG_ENABLED)
		{
			FColor debugColor = (interactableHitActor == NULL)
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

	if (DEBUG_ENABLED)
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
	outTraceResult.HitActor = closestInteractable;
	outTraceResult.HitDistance = closestActorDistance;

	return (closestInteractable != NULL);
}

int ACaravanCharacter::GetCraftResourceCount(ECraftResourceType resourceType) const
{
	return CraftResourceHelpers::IsValidType(resourceType) ? CraftResourceCount[(uint8)resourceType] : 0;
}