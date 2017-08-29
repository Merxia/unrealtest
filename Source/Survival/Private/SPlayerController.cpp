// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"
#include "SPlayerCameraManager.h"

ASPlayerController::ASPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();
}

