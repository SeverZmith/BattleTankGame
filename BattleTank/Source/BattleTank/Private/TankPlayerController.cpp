// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "TankAimingComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Tank.h"

// Annotate out parameters
#define OUT

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!GetPawn())
	{
		return;

	}
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent))
	{
		return;

	}
	FoundAimingComponent(AimingComponent);

}

void ATankPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank))
		{
			return;
		}
		// Subscribe to tank death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}
}

void ATankPlayerController::OnPossessedTankDeath()
{
	StartSpectatingOnly();

}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimTowardsCrosshair();

}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetPawn()) // e.g. if not possessing
	{
		return;

	}
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent))
	{
		return;

	}

	FVector HitLocation; /// Out Parameter
	bool bGotHitLocation = GetSightRayHitLocation(OUT HitLocation);
	if (bGotHitLocation)
	{
		AimingComponent->AimAt(HitLocation);

	}

}

bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	// Find the crosshair position
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(OUT ViewportSizeX, OUT ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, OUT LookDirection))
	{
		// Line-trace that LookDirection and see what we hit (up to max range)
		return GetLookVectorHitLocation(LookDirection, OUT HitLocation);

	}
	return false;

}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	// Declare HitResult out parameter. Set StartLocation to camera position.
	//		Set EndLocation to Start + (direction you're looking * line trace max range)
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
			OUT HitResult,
			StartLocation,
			EndLocation,
			ECollisionChannel::ECC_Camera)
		)
	{
		HitLocation = HitResult.Location;
		return true;

	}
	// Sets HitLocation to 0 if looking off into space
	HitLocation = FVector(0);
	return false;

}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation;
	return DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, CameraWorldLocation, OUT LookDirection);

}
