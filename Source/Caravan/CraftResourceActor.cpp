// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftResourceActor.h"
#include "Caravan.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "RPG/InventoryComponent.h"
#include "Utils/CaravanEngineUtils.h"

static TAutoConsoleVariable<bool> CVarAIDebug_UsePlayerInventory(
	TEXT("AI.Robot.UsePersonalInventory"),
	true,
	TEXT("Toggle AI Should store items in Player's inventory rather than personal"),
	ECVF_Cheat);

// Sets default values
ACraftResourceActor::ACraftResourceActor(const class FObjectInitializer& ObjInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CraftResourceActor_StaticMeshComponent"));
}

// Called when the game starts or when spawned
void ACraftResourceActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACraftResourceActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

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

FString ACraftResourceActor::GetInteractionName() const
{ 
	return FString::Printf(TEXT("COLLECT %s"), *GetResourceName().ToUpper());
}

void ACraftResourceActor::OnInteractFocus(const InteractData& interactData)
{
	AInteractableActor::OnInteractFocus(interactData);
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

EInteractionType ACraftResourceActor::OnInteractSelect(const InteractData& interactData)
{
	AInteractableActor::OnInteractSelect(interactData);
	GEngine->AddOnScreenDebugMessage(1, 2.5f, FColor::Cyan, FString::Printf(TEXT("COLLECTED %s"), *GetResourceName().ToUpper()));

	if (interactData.Pawn != NULL)
	{
		APawn* pawnWithInventory = NULL;
		if (CVarAIDebug_UsePlayerInventory.GetValueOnGameThread() == true)
		{
			if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
			{
				if (APawn* playerPawn = Cast<APawn>(playerController->GetPawn()))
				{
					pawnWithInventory = playerPawn;
				}
			}
		}
		else
		{
			pawnWithInventory = interactData.Pawn;
		}

		if (UInventoryComponent* const inventory = FindInventoryComponent(pawnWithInventory))
		{
			inventory->AddCraftResource(this);
		}
	}

	Destroy();
	return EInteractionType::ResourceCollect;
}

bool CraftResourceHelpers::IsValidType(ECraftResourceType type)
{
	return (type < ECraftResourceType::Invalid);
}