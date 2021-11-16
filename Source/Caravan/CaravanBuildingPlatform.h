// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractableActor.h"
#include "CaravanBuildingPlatform.generated.h"

class UInteractableComponent;
class UStaticMeshSocket;

UCLASS()
class ACaravanBuildingPlatform : public AActor
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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractableComponent* InteractableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

private:
	bool IsActive{ true }; // Reset to false in BeginPlay
};
