#pragma once

#include "CoreMinimal.h"

class AActor;
class UCharacterSpec;

namespace CaravanSpawnUtils
{
	AActor* SpawnActor(UObject* WorldContextObject, const UCharacterSpec* Spec, const FTransform& Transform);
	AActor* SpawnActor(UObject* WorldContextObject, const UCharacterSpec* Spec, const FVector& Location, const FRotator& Rotation, const FVector& Scale = FVector::OneVector);

} // CaravanSpawnUtils