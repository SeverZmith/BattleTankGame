// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"


ATank::ATank()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATank::BeginPlay()
{
	Super::BeginPlay();

	// Set health pool.
	CurrentHealth = StartingHealth;

}

float ATank::GetHealthPercentage() const
{
	return (float)CurrentHealth / (float)StartingHealth;

}

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	// Clamp the amount of damage received as an int between 0 and our CurrentHealth.
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth);

	// Apply damage to health pool and broadcast death if health drops to 0.
	CurrentHealth -= DamageToApply;
	if (CurrentHealth <= 0)
	{
		OnDeath.Broadcast();

	}
	UE_LOG(LogTemp, Warning, TEXT("Current Health = %i\nDamageAmount = %f, DamageToApply = %i"), CurrentHealth, DamageAmount, DamageToApply);

	return DamageToApply;

}
