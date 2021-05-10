// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiToolActor.h"
#include "Caravan.h"
#include "Engine.h"

static const FName SOCKET_CARRY("CarrySocket");

// Sets default values
AMultiToolActor::AMultiToolActor(const class FObjectInitializer& ObjInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = ObjInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MultiToolActor_StaticMeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MultiToolMesh(TEXT("/Game/ThirdPerson/Meshes/MultiToolStaticMesh"));
	if (StaticMeshComponent && MultiToolMesh.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MultiToolMesh.Object);
	}
}

// Called when the game starts or when spawned
void AMultiToolActor::BeginPlay()
{
	Super::BeginPlay();

	// HACK: Offset pivot position by the carry socket's relative location.
	// This can be avoided by fixing the pivot position of the MultiTool's mesh in 3ds
	if (const UStaticMeshSocket* pCarrySocket = StaticMeshComponent->GetSocketByName(SOCKET_CARRY))
	{
		StaticMeshComponent->AddLocalOffset(-pCarrySocket->RelativeLocation);
	}
}

// Called every frame
void AMultiToolActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

