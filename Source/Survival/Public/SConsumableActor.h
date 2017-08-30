// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPickupActor.h"
#include "SConsumableActor.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVAL_API ASConsumableActor : public ASPickupActor
{
	GENERATED_UCLASS_BODY()
protected:

	/* Consume item, restoring energy to player */
	void OnUsed(APawn* InstigatorPawn) override;

	/* Amount of hitpoints restored and hunger reduced when consumed. */
	UPROPERTY(EditDefaultsOnly, Category = "Consumable")
	float Nutrition;
	

};
