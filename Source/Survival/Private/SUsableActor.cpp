// Fill out your copyright notice in the Description page of Project Settings.

#include "SUsableActor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


ASUsableActor::ASUsableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComp = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
}


void ASUsableActor::OnUsed(APawn* InstigatorPawn)
{
	// Nothing to do here...
}


void ASUsableActor::OnBeginFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(true);
}


void ASUsableActor::OnEndFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(false);
}

