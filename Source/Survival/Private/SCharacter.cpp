// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SCharacterMovementComponent.h"
#include "SUsableActor.h"
#include "SHUD.h"

#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Engine.h"

// Sets default values
ASCharacter::ASCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Adjust jump to make it less floaty
	MoveComp->GravityScale = 1.5f;
	MoveComp->JumpZVelocity = 620;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200;

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SetupAttachment(GetRootComponent());
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->TargetArmLength = 300.0f;
	CameraBoomComp->bUsePawnControlRotation = true;
	

	CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	CameraComp->SetupAttachment(CameraBoomComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	MaxUseDistance = 800;
	bHasNewFocus = true;
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;

	IncrementHungerAmount = 1.0f;
	IncrementHungerInterval = 5.0f;
	CriticalHungerThreshold = 90;
	MaxHunger = 100;
	Hunger = 0;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Set a timer to increment hunger every interval
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ASCharacter::IncrementHunger, IncrementHungerInterval, true);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}

	if (Controller && Controller->IsLocalController())
	{
		ASUsableActor* Usable = GetUsableInView();

		// End Focus
		if (FocusedUsableActor != Usable)
		{
			if (FocusedUsableActor)
			{
				FocusedUsableActor->OnEndFocus();
			}

			bHasNewFocus = true;
		}

		// Assign new Focus
		FocusedUsableActor = Usable;

		// Start Focus.
		if (Usable)
		{
			if (bHasNewFocus)
			{
				Usable->OnBeginFocus();
				bHasNewFocus = false;
			}
		}
	}

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ASCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &ASCharacter::OnStopSprinting);

	InputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrouchToggle);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);

	InputComponent->BindAction("Use", IE_Pressed, this, &ASCharacter::Use);

	InputComponent->BindAction("Targeting", IE_Pressed, this, &ASCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &ASCharacter::OnEndTargeting);

}

void ASCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}

void ASCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

/*
Performs ray-trace to find closest looked-at UsableActor.
*/
ASUsableActor* ASCharacter::GetUsableInView()
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == NULL)
		return NULL;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	return Cast<ASUsableActor>(Hit.GetActor());
}


void ASCharacter::Use()
{
	ASUsableActor* Usable = GetUsableInView();
	if (Usable)
	{
		Usable->OnUsed(this);
	}
}

void ASCharacter::OnStartTargeting()
{
	SetTargeting(true);
}

void ASCharacter::OnEndTargeting()
{
	SetTargeting(false);
}

void ASCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;
}

bool ASCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float ASCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

FRotator ASCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

float ASCharacter::GetHealth() const
{
	return Health;
}

float ASCharacter::GetHunger() const
{
	return Hunger;
}

float ASCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASCharacter>()->Health;
}

float ASCharacter::GetMaxHunger() const
{
	return MaxHunger;
}

void ASCharacter::ConsumeFood(float AmountRestored)
{
	// Reduce Hunger, ensure we do not go outside of our bounds
	Hunger = FMath::Clamp(Hunger - AmountRestored, 0.0f, GetMaxHunger());

	// Restore Hitpoints
	Health = FMath::Clamp(Health + AmountRestored, 0.0f, GetMaxHealth());

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		ASHUD* MyHUD = Cast<ASHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->MessageReceived("Food item consumed!");
		}
	}

}

bool ASCharacter::IsAlive() const
{
	return Health > 0;
}

void ASCharacter::IncrementHunger()
{
	Hunger = FMath::Clamp(Hunger + IncrementHungerAmount, 0.0f, GetMaxHunger());

	if (Hunger > CriticalHungerThreshold)
	{
		// Apply damage to self.
		// TODO: Set DamageType
		TakeDamage(10.0f, FDamageEvent(), GetController(), this);
	}
}

float ASCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			// TODO: Handle death
			//Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			// TODO: Play hit
			//PlayHit(ActualDamage, DamageEvent, EventInstigator->GetPawn(), DamageCauser, false);
		}
	}

	return ActualDamage;
}

void ASCharacter::OnStartJump()
{
	bPressedJump = true;

	SetIsJumping(true);
}

void ASCharacter::OnStopJump()
{
	bPressedJump = false;
}

void ASCharacter::Landed(const FHitResult& Hit)
{
	Super::OnLanded(Hit);

	SetIsJumping(false);
}

bool ASCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ASCharacter::SetIsJumping(bool NewJumping)
{
	// Go to standing pose if trying to jump while crouched
	if (bIsCrouched && NewJumping)
	{
		UnCrouch();
	}
	else
	{
		bIsJumping = NewJumping;
	}
}

void ASCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
		UnCrouch();
}


void ASCharacter::OnStartSprinting()
{
	SetSprinting(true);
}


void ASCharacter::OnStopSprinting()
{
	SetSprinting(false);
}


bool ASCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
		return false;

	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero()
		// Don't allow sprint while strafing sideways or standing still (1.0 is straight forward, -1.0 is backward while near 0 is sideways or standing still)
		&& (GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > 0.8; // Changing this value to 0.1 allows for diagonal sprinting. (holding W+A or W+D keys)
}


float ASCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}

void ASCharacter::OnCrouchToggle()
{
	// If we are crouching then CanCrouch will return false. If we cannot crouch then calling Crouch() wont do anything
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

