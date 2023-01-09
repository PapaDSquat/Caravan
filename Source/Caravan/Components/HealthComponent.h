#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class APawn;
class UInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealthComponentDamageEvent, UHealthComponent*, HealthComponent, AActor*, Instigator, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthComponentKillEvent, UHealthComponent*, HealthComponent, AActor*, Instigator);

UCLASS(BlueprintType)
class CARAVAN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplyDamage(AActor* Instigator, float Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void Kill(AActor* Instigator);


	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FHealthComponentDamageEvent OnDamage;

	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FHealthComponentKillEvent OnKill;
	
protected:
	UPROPERTY(SaveGame)
	float Health = 100.f;
};
