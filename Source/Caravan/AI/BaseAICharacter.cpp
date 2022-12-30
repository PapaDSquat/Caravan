#include "AI/BaseAICharacter.h"

#include "AI/AICharacterSpec.h"
#include "Components/InteractableComponent.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjInitializer)
{
	FindOrCreateComponent(UInventoryComponent, InventoryComponent, "InventoryComponent")
	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->SetupAttachment(RootComponent);
	}
}