// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class ACaravanCharacter;

UCLASS()
class CARAVAN_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsMoving;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	bool IsInteracting;

public:
	UPlayerAnimInstance(const class FObjectInitializer& ObjInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

private:
	ACaravanCharacter* OwningCharacter;
};
