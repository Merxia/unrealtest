// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

	/* Update the FOV */
	virtual void UpdateCamera(float DeltaTime) override;

public:

	/* default, hip fire FOV */
	float NormalFOV;

	/* aiming down sight / zoomed FOV */
	float TargetingFOV;
};
