#include "PlayerAnimInstance.h"
#include "Caravan.h"
#include "CaravanCharacter.h"
#include "Components/InteractionComponent.h"
#include "GameFramework/Pawn.h"

UPlayerAnimInstance::UPlayerAnimInstance(const class FObjectInitializer& ObjInitializer)
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (IsValid(OwningCharacter))
	{
		InteractionComponent = OwningCharacter->FindComponentByClass<UInteractionComponent>();
	}
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
	if (InteractionComponent)
	{
		IsInteracting = InteractionComponent->IsInteracting();
	}
}