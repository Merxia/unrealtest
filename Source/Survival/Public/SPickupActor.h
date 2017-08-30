// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUsableActor.h"
#include "SPickupActor.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVAL_API ASPickupActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()
	
	
	void BeginPlay() override;

	void OnUsed(APawn* InstigatorPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundCue* PickupSound;
	
};
