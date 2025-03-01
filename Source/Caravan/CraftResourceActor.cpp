// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftResourceActor.h"
#include "Caravan.h"
#include "Components/InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

// Sets default values
ACraftResourceActor::ACraftResourceActor(const class FObjectInitializer& ObjInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CraftResourceActor_StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	FindOrCreateComponent(UInteractableComponent, InteractableComponent, "InteractableComponent")
	{
		InteractableComponent->SetupAttachment(RootComponent);
	}
}

void ACraftResourceActor::BeginPlay()
{
	Super::BeginPlay();

	// Event Registration
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ACraftResourceActor::OnInteract);
	}
}

/*static*/ /*bool ACraftResourceActor::CreateInWorld(const SCraftResourceInitData& initData, ACraftResourceActor* pOutActor)
{
	FActorSpawnParameters SpawnParameters;
	pOutActor = GetWorld()->SpawnActor<ACraftResourceActor>(SpawnParameters);
	if (pOutActor)
	{
		pOutActor->InitCraftResource(initData);
	}
	return (logResourceActor != NULL);
}*/

void ACraftResourceActor::InitCraftResource(const SCraftResourceInitData& initData)
{
	ResourceType = initData.Type;

	// Assert
	if (CraftResourceHelpers::IsValidType(ResourceType))
	{
		ResourceName = CaravanUtils::EnumToString(ResourceType);
		
		if (StaticMeshComponent)
		{
			FName meshName = FName(*FString::Printf(TEXT("/Game/Caravan/Meshes/CraftResource_%s_StaticMesh"), *ResourceName));
			if (UStaticMesh* ResourceMesh = DynamicLoadObjFromPath<UStaticMesh>(meshName))
			{
				StaticMeshComponent->SetStaticMesh(ResourceMesh);
				StaticMeshComponent->SetSimulatePhysics(true);
				StaticMeshComponent->WakeRigidBody();
			}
		}
	}

	SetActorLocation(initData.Location);
	SetActorRotation(initData.Rotation);
}

// TODO: Probably shouldn't need this
UInventoryComponent* const FindInventoryComponent(const APawn* inPawn)
{
	if (inPawn != NULL)
	{
		if (UInventoryComponent* const inventory = inPawn->FindComponentByClass< UInventoryComponent >())
		{
			return inventory;
		}

		if (const AController* pawnController = inPawn->GetController< AController>())
		{
			if (UInventoryComponent* const inventory = pawnController->FindComponentByClass< UInventoryComponent >())
			{
				return inventory;
			}
		}
	}
	return NULL;
}

void ACraftResourceActor::OnInteract(APawn* InteractingPawn, UInteractableComponent* Interactable, const FInteractionChoice& Choice)
{
	GEngine->AddOnScreenDebugMessage(1, 2.5f, FColor::Cyan, FString::Printf(TEXT("COLLECTED %s"), *GetResourceName().ToUpper()));

	if (InteractingPawn != NULL)
	{
		UInventoryComponent* InventoryComponent = nullptr;

		// Local inventory?
		// InventoryComponent = InteractingPawn->FindComponentByClass<UInventoryComponent>();

		// Always deposit into player inventory
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = Cast<APawn>(PlayerController->GetPawn()))
			{
				InventoryComponent = PlayerPawn->FindComponentByClass<UInventoryComponent>();
			}
		}

		if (InventoryComponent)
		{
			InventoryComponent->AddCraftResource(this);
		}
	}

	Destroy();
}

/*
FString ACraftResourceActor::GetInteractionName() const
{
	return FString::Printf(TEXT("COLLECT %s"), *GetResourceName().ToUpper());
}
*/

bool CraftResourceHelpers::IsValidType(ECraftResourceType type)
{
	return (type < ECraftResourceType::Invalid);
}