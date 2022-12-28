#pragma once

#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class ACaravanCharacter;
class UInteractionComponent;

UCLASS()
class CARAVAN_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance(const class FObjectInitializer& ObjInitializer);

	// BEGIN UAnimInstance
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	// END UAnimInstance

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsMoving = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	bool IsInteracting = false;

private:
	UPROPERTY(Transient)
	ACharacter* OwningCharacter;

	UPROPERTY(Transient)
	UInteractionComponent* InteractionComponent;
};
