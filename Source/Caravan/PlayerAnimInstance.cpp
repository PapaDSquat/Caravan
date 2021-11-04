// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAnimInstance.h"
#include "Caravan.h"
#include "CaravanCharacter.h"
#include "GameFramework/Pawn.h"


UPlayerAnimInstance::UPlayerAnimInstance(const class FObjectInitializer& ObjInitializer)
{
	
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningCharacter)
	{
		return;
	}

	// MOVEMENT
	IsMoving = (OwningCharacter->GetVelocity().SizeSquared() > 25);

	if (UPawnMovementComponent* pMovementComponent = OwningCharacter->GetMovementComponent())
	{
		IsFalling = (pMovementComponent->IsFalling());
	}

	Speed = OwningCharacter->GetVelocity().Size();

	// INTERACTION
	// TODO : Get from InteractableComponent
	// IsInteracting = OwningCharacter->Interacting;
}