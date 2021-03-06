// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class SURVIVAL_API ASCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

private:
	/* Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoomComp;

	/* Primary camera of the player*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;

public:	

	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StopAllAnimMontages();

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	void MoveForward(float Val);

	void MoveRight(float Val);

	void OnStartJump();

	void OnStopJump();

	void OnStartSprinting();

	void OnStopSprinting();

	void OnCrouchToggle();

	virtual void Landed(const FHitResult& Hit) override;
	
	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient)
	bool bWantsToRun;

	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	/* Client/local call to update sprint state  */
	void SetSprinting(bool NewSprinting);


	UFUNCTION(BlueprintCallable, Category = Movement)
	bool IsSprinting() const;

	float GetSprintingSpeedModifier() const;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;


	/************************************************************************/
	/* Object Interaction                                                   */
	/************************************************************************/

	virtual void Use();

	class ASUsableActor* GetUsableInView();

	UPROPERTY(EditDefaultsOnly, Category = "ObjectInteraction")
	float MaxUseDistance;

	/* True only in first frame when focused on a new usable actor. */
	bool bHasNewFocus;

	class ASUsableActor* FocusedUsableActor;

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void OnStartTargeting();

	void OnEndTargeting();

	void SetTargeting(bool NewTargeting);

	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	UPROPERTY(Transient)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/************************************************************************/
	/* Hitpoints & Hunger                                                   */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	void ConsumeFood(float AmountRestored);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	/* Increments hunger, used by timer. */
	void IncrementHunger();

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerAmount;

	/* Limit when player suffers Hitpoints from extreme hunger */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float CriticalHungerThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Hunger;

	// Documentation Note: MaxHunger does not need to be replicated, only values that change and are displayed or used by clients should ever be replicated.
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float MaxHunger;

	/************************************************************************/
	/* Damage, Hit & Death                                                  */
	/************************************************************************/

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	void SetRagdollPhysics();

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	bool bIsDying;
};
