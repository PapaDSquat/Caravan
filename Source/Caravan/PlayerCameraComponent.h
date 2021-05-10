// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "PlayerCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class CARAVAN_API UPlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
	
	
	// TODO: Array of Camera data, mapped to Player State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FTransform TargetCameraMoveOffset;
};
