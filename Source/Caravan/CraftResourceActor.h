// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractableActor.h"
#include "CraftResourceActor.generated.h"

UENUM(BlueprintType)
enum ECraftResourceType
{
	Wood,
	Stone,
	MAX
};

static const CHAR* CRAFT_RESOURCE_NAME[ECraftResourceType::MAX] =
{
	"Wood",
	"Stone",
};

struct CraftResourceHelpers
{
	static bool IsValidType(ECraftResourceType type);
};

struct SCraftResourceInitData
{
	ECraftResourceType Type{ ECraftResourceType::MAX };
	FVector Location;
	FRotator Rotation;
};

UCLASS()
class CARAVAN_API ACraftResourceActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	//static bool CreateInWorld(const SCraftResourceInitData& initData, ACraftResourceActor* pOutActor);
	void InitCraftResource(const SCraftResourceInitData& initData);
	
	// Sets default values for this actor's properties
	ACraftResourceActor(const class FObjectInitializer& ObjInitializer);

	const FString& GetResourceName() const { return ResourceName; }

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// IInteractable
	virtual FString GetInteractionName() const override;
	virtual ECraftResourceType GetResourceType() const override { return ResourceType; }
	virtual void OnInteractFocus(const InteractData& interactData) override;
	virtual EInteractionType OnInteractSelect(const InteractData& interactData) override;

private:

	ECraftResourceType ResourceType;
	FString ResourceName;
	UStaticMeshComponent* StaticMeshComponent;
};
