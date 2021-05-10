// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MultiToolActor.generated.h"

UCLASS()
class CARAVAN_API AMultiToolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultiToolActor(const class FObjectInitializer& ObjInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
protected:
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
};
