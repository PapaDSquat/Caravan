// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InteractableComponent.h"
#include "CraftResourceActor.generated.h"

UENUM(BlueprintType)
enum class ECraftResourceType : uint8
{
	Wood,
	Stone,
	Invalid
};

struct CraftResourceHelpers
{
	static bool IsValidType(ECraftResourceType type);
};

struct SCraftResourceInitData
{
	ECraftResourceType Type{ ECraftResourceType::Invalid };
	FVector Location;
	FRotator Rotation;
};

UCLASS()
class CARAVAN_API ACraftResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual void BeginPlay() override;

	//static bool CreateInWorld(const SCraftResourceInitData& initData, ACraftResourceActor* pOutActor);
	void InitCraftResource(const SCraftResourceInitData& initData);
	
	// Sets default values for this actor's properties
	ACraftResourceActor(const class FObjectInitializer& ObjInitializer);

	const FString& GetResourceName() const { return ResourceName; }

	virtual ECraftResourceType GetResourceType() const { return ResourceType; }
	//virtual FString GetInteractionName() const override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, Category = "Resource")
	ECraftResourceType ResourceType;

	UPROPERTY(EditAnywhere, Category = "Resource")
	FString ResourceName;

private:
	UFUNCTION()
	void OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice);

};
