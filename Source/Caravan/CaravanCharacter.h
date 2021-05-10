// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "CraftResourceActor.h"
#include "CaravanCharacter.generated.h"

class AInteractableActor;
class ACaravanActor;
class AMultiToolActor;
class AWorldGenerator;

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

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	int GetCraftResourceCount(ECraftResourceType resourceType) const;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Interact();

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

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate = 25.f;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractLength = 275.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
		AWorldGenerator* WorldGenerator = NULL;

	// Targeting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		bool IsTargeting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		AInteractableActor* TargetActor = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		FVector TargetBaseLocation;

	// Interaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		AInteractableActor* InteractFocus = NULL;


	// TODO: Move to settings file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool DEBUG_ENABLED = false;

private:
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
		AInteractableActor* HitActor;
		float HitDistance;
		SInteractTraceResult& operator = (const SInteractTraceResult& rhs)
		{
			HitResult = rhs.HitResult;
			HitActor = rhs.HitActor;
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

	// Crafting (TODO: Make it a manager)
	int CraftResourceCount[ECraftResourceType::MAX];
};

