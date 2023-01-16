#include "AI/RobotAIController.h"

#include "AI/RobotAICharacter.h"
#include "AI/AIRobotSubsystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/InteractableComponent.h"
#include "Components/InteractionComponent.h"
#include "Utils/CaravanEngineUtils.h"

ARobotAIController::ARobotAIController(const FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer)
{
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!IsValid(RobotCharacter))
		return;

	// Event Registration
	if (UInteractableComponent* InteractableComponent = RobotCharacter->GetInteractableComponent())
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!RobotCharacter)
		return;

	// Event Unregistration
	if (UInteractableComponent* InteractableComponent = RobotCharacter->GetInteractableComponent())
	{
		InteractableComponent->OnInteract.RemoveDynamic(this, &ARobotAIController::OnInteract);
	}
}

void ARobotAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARobotAIController::OnRegisterToSubsystem()
{
	OnAIRobotRegister.Broadcast(this);

	ARobotAICharacter* RobotCharacter = GetRobotOwner();
	if (!RobotCharacter)
		return;

	// Setup Interactable
	if (UInteractableComponent* InteractableComponent = RobotCharacter->GetInteractableComponent())
	{
		InteractableComponent->InteractableObjectName = FText::FromName(CharacterProfile.Name);
		InteractableComponent->InteractableObjectSubTitle = FText::FromString(CaravanUtils::EnumToString(CharacterProfile.PrimarySkill));
	}
}

void ARobotAIController::OnUnregisterFromSubsystem()
{
	OnAIRobotUnregister.Broadcast(this);
}

void ARobotAIController::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	// TODO	
}

ARobotAICharacter* ARobotAIController::GetRobotOwner() const
{
	return Cast< ARobotAICharacter >(GetCharacter());
}

FName ARobotAIController::GetRobotName() const
{
	return CharacterProfile.Name;
}

ERobotAILocale ARobotAIController::GetCurrentAILocale() const
{
	return RobotState.Locale;
}

void ARobotAIController::SetCurrentAILocale(ERobotAILocale NewLocale)
{
	const ERobotAILocale OldLocale = RobotState.Locale;
	RobotState.Locale = NewLocale;
	OnAILocaleChanged(OldLocale, NewLocale);
}

bool ARobotAIController::GetIsOnExpedition() const
{
	return RobotState.bFollowPlayerRequested;
}

void ARobotAIController::SetIsOnExpedition(bool bIsOnExpedition)
{
	RobotState.bFollowPlayerRequested = bIsOnExpedition;
}

bool ARobotAIController::Interact(UInteractableComponent* Interactable)
{
	if (ARobotAICharacter* RobotCharacter = GetRobotOwner())
	{
		return RobotCharacter->InteractionComponent->Interact(RobotCharacter, Interactable);
	}
	return false;
}

bool ARobotAIController::GetIsFollowingPlayer() const
{
	return Blackboard ? Blackboard->GetValueAsBool(BB_IsFollowingPlayer) : false;
}

FText ARobotAIController::GetBehaviorTreeActiveTaskDescriptor() const
{
	UBehaviorTreeComponent* BehaviorTree = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTree)
	{
		return FText::FromString(BehaviorTree->DescribeActiveTasks());
	}
	return FText::GetEmpty();
}

FText ARobotAIController::GetActiveTaskDescriptor() const
{
	return ActiveTaskDescriptor;
}

void ARobotAIController::SetActiveTaskDescriptor(const FText& InText)
{
	ActiveTaskDescriptor = InText;
}