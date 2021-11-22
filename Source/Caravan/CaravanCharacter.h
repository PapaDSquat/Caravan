// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "CraftResourceActor.h"
#include "CaravanCharacter.generated.h"

class UInteractableComponent;
class ACaravanActor;
class AMultiToolActor;

UCLASS(config=Game)
class ACaravanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACaravanCharacter(const class FObjectInitializer& ObjInitializer);

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	bool Interacting{ false };

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsCarryingCaravan() const { return (ActiveCaravan != NULL); }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetFocusedActor() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetTargetedActor() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UInteractableComponent* GetFocusedInteractable() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UInteractableComponent* GetTargetedInteractable() const;

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	//void SetInteractFocus()
	void InteractSelect();
	void InteractChoiceScrollUp();
	void InteractChoiceScrollDown();

	void OnTargetActivate();
	void OnTargetDeactivate();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void DEBUG_ResetResourceGrid();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Dwindle
	// True if Player hasn't moved outside of a fixed range within a fixed amount of time
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool IsLoitering = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float LoiterRange = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float LoiterTime = 5.f;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate = 25.f;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractLength = 275.f;

	// Targeting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool IsTargeting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool IsTargetingWithChoices = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractableComponent* InteractTarget = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FVector TargetBaseLocation;

	// Interaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractableComponent* InteractFocus = NULL;

	UPROPERTY(BlueprintReadOnly, Category = "RPG")
	class UInventoryComponent* Inventory;

private:
	void OnInteractComponentDeactivated(UActorComponent* Component, bool bReset);

	void UpdateDwindleState(float DeltaSeconds);

	struct SInteractTraceData
	{
		SInteractTraceData(FVector start, FVector end)
			: Start(start), End(end) {}
		FVector Start;
		FVector End;
	};
	struct SInteractTraceResult
	{
		FHitResult HitResult;
		UInteractableComponent* HitComponent;
		float HitDistance;
		SInteractTraceResult& operator = (const SInteractTraceResult& rhs)
		{
			HitResult = rhs.HitResult;
			HitComponent = rhs.HitComponent;
			HitDistance = rhs.HitDistance;
			return *this;
		}
	};
	bool TryInteractTrace(const TArray<SInteractTraceData>& traceDataList, SInteractTraceResult& outTraceResult);

	SInteractTraceResult LastTraceResult;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Movement
	ACaravanActor* ActiveCaravan{ nullptr };
	AMultiToolActor* MultiToolActor{ nullptr };

	// Dwindle
	struct FDwindleState
	{
		float TimeBeforeReset = 0.f;
		FVector LastActorLocation;
	};
	FDwindleState DwindleState;
};

