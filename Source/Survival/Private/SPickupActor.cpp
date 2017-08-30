// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ASPickupActor::ASPickupActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// All objects that can be picked up are simulated and can be thrown around the level
	MeshComp->SetSimulatePhysics(true);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}

void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Remove Hack to workaround constructor call not currently working.
	MeshComp->SetSimulatePhysics(true);
}

void ASPickupActor::OnUsed(APawn * InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}
