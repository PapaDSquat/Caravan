#pragma once

#include "AI/AICharacterSpec.h"
#include "AIController.h"
#include "CoreMinimal.h"
#include "RobotAIController.generated.h"

class UInteractableComponent;
enum class ERobotAILocale : uint8;
struct FInteractionChoice;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRobotAIRegisterEvent, class ARobotAIController*, AIController);

USTRUCT(BlueprintType)
struct CARAVAN_API FRobotAIState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AI State")
	bool bFollowPlayerRequested = false;

	UPROPERTY(BlueprintReadOnly, Category = "AI State")
	ERobotAILocale Locale = ERobotAILocale::Invalid;
};

UCLASS()
class CARAVAN_API ARobotAIController : public AAIController
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void BeginPlay() override;

	void OnRegisterToSubsystem();
	void OnUnregisterFromSubsystem();

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	ARobotAICharacter* GetRobotOwner() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	FName GetRobotName() const;

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	ERobotAILocale GetCurrentAILocale() const;

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	void SetCurrentAILocale(ERobotAILocale NewLocale);

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	bool GetIsOnExpedition() const;

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	void SetIsOnExpedition( bool bIsOnExpedition );

	UFUNCTION(BlueprintCallable, Category = "AI|Interaction")
	bool Interact(UInteractableComponent* Interactable);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAILocaleChanged(ERobotAILocale OldLocale, ERobotAILocale NewLocale);

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool GetIsFollowingPlayer() const;

	// Get text description of the active task from the behavior tree
	UFUNCTION(BlueprintCallable, Category = "AI")
	FText GetBehaviorTreeActiveTaskDescriptor() const;

	// Get the custom description of the active task
	UFUNCTION(BlueprintCallable, Category = "AI")
	FText GetActiveTaskDescriptor() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetActiveTaskDescriptor(const FText& InText);

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FRobotAIProfile CharacterProfile;
	
	UPROPERTY(BlueprintReadOnly, Category = "AI|State")
	FRobotAIState RobotState;

	UPROPERTY(BlueprintAssignable)
	FRobotAIRegisterEvent OnAIRobotRegister;

	UPROPERTY(BlueprintAssignable)
	FRobotAIRegisterEvent OnAIRobotUnregister;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
	FName BB_IsFollowingPlayer = TEXT("FollowPlayerRequested");

	FText ActiveTaskDescriptor;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);
};
