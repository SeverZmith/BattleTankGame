// Fill out your copyright notice in the Description page of Project Settings.

#include "TankTrack.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "SprungWheel.h"
#include "SpawnPoint.h"



UTankTrack::UTankTrack()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UTankTrack::SetThrottle(float Throttle)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	DriveTracks(CurrentThrottle);

}

void UTankTrack::DriveTracks(float CurrentThrottle)
{
	// Clamp actual throttle value to prevent overdriving
	auto ForceApplied = CurrentThrottle * TrackMaxDrivingForce;
	auto Wheels = GetWheels();
	auto ForcePerWheel = ForceApplied / Wheels.Num();
	for (ASprungWheel* Wheel : Wheels)
	{
		Wheel->AddDrivingForce(ForcePerWheel);

	}

}

TArray<ASprungWheel*> UTankTrack::GetWheels() const
{
	TArray<ASprungWheel*> ResultArray;
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		auto SpawnPointChild = Cast<USpawnPoint>(Child);
		if (!SpawnPointChild)
		{
			continue;

		}
		AActor* SpawnedChild = SpawnPointChild->GetSpawnedActor();
		auto SprungWheel = Cast<ASprungWheel>(SpawnedChild);
		if (!SprungWheel)
		{
			continue;

		}
		ResultArray.Add(SprungWheel);

	}
	return ResultArray;

}
