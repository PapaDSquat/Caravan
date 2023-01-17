#include "AI/BaseAICharacter.h"

#include "AI/AICharacterSpec.h"
#include "Components/HealthComponent.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjInitializer)
{
	FindOrCreateComponent(UHealthComponent, HealthComponent, "HealthComponent")
	FindOrCreateComponent(UInventoryComponent, InventoryComponent, "InventoryComponent")
	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->SetupAttachment(RootComponent);
	}
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnKill.AddDynamic(this, &ABaseAICharacter::OnKill);
}

void ABaseAICharacter::SetWalkSpeed(float WalkSpeed)
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
	}
}

void ABaseAICharacter::OnKill(UHealthComponent* InHealthComponent, AActor* InInstigator)
{
	InventoryComponent->DropAllItems();

	Destroy();
}