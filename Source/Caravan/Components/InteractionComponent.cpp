#include "Components/InteractionComponent.h"

#include "Components/InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Interacting = false;
}

void UInteractionComponent::Interact(ACharacter* InteractingCharacter, UInteractableComponent* Interactable)
{
	if (!InteractingCharacter || !Interactable)
		return;

	Interacting = true; // For one frame

	Interactable->Interact(InteractingCharacter);
}

AActor* UInteractionComponent::GetFocusedActor() const
{
	return IsValid(InteractFocus) ? InteractFocus->GetOwner() : NULL;
}

AActor* UInteractionComponent::GetTargetedActor() const
{
	return IsValid(InteractTarget) ? InteractTarget->GetOwner() : NULL;
}