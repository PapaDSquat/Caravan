// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractableActor.h"
#include "CaravanBuildingPlatform.generated.h"

class UStaticMeshSocket;

UCLASS()
class ACaravanBuildingPlatform : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaravanBuildingPlatform(const class FObjectInitializer& ObjInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	bool GetActive() const { return IsActive; }
	void SetActive(bool bActive);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	bool IsActive{ true }; // Reset to false in BeginPlay
};
