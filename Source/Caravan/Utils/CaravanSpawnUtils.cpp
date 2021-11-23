#include "CaravanSpawnUtils.h"
#include "AI/AICharacterSpec.h"
#include "AI/RobotAICharacter.h"
#include "AI/RobotAIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

namespace CaravanSpawnUtils
{
	AActor* SpawnActor(UObject* WorldContextObject, const UCharacterSpec* Spec, const FTransform& Transform)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (!World || !Spec || !Spec->CharacterClass)
		{
			// TODO: Log
			return NULL;
		}

		FActorSpawnParameters ActorSpawnParams;
		//ActorSpawnParams.Owner = Owner;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* SpawnedActor = World->SpawnActorDeferred<APawn>(Spec->CharacterClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (APawn* Pawn = Cast< APawn >(SpawnedActor))
		{
			if (Pawn->Controller == NULL)
			{
				// NOTE: SpawnDefaultController ALSO calls Possess() to possess the pawn (if a controller is successfully spawned).
				Pawn->SpawnDefaultController();
			}
		}

		// TODO: Possibly provide functor to call here

		if (SpawnedActor)
		{
			UGameplayStatics::FinishSpawningActor(SpawnedActor, Transform);
		}

		return SpawnedActor;
	}
	AActor* SpawnActor(UObject* WorldContextObject, const UCharacterSpec* Spec, const FVector& Location, const FRotator& Rotation, const FVector& Scale /*= FVector::OneVector*/)
	{
		const FTransform Transform(Rotation.Quaternion(), Location, Scale);
		return SpawnActor(WorldContextObject, Spec, Transform);
	}

} // CaravanSpawnUtils