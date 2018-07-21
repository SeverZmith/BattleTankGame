// Copyright SeverZmith

#include "SprungWheel.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"


ASprungWheel::ASprungWheel()
{
	// Change TickGroup here because we want our Tick to occur after the OnHit. Default TickGroup is TG_PrePhysics.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostPhysics;

	// Create the UPhysicsConstraintComponent to simulate a car suspension.
	MassWheelConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("MassWheelConstraint"));
	SetRootComponent(MassWheelConstraint);

	// Create the USphereComponent to simulate a wheel and axle of a car.
	Axle = CreateDefaultSubobject<USphereComponent>(FName("Axle"));
	Axle->SetupAttachment(MassWheelConstraint);

	Wheel = CreateDefaultSubobject<USphereComponent>(FName("Wheel"));
	Wheel->SetupAttachment(Axle);

	AxleWheelConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("AxleWheelConstraint"));
	AxleWheelConstraint->SetupAttachment(Axle);

}

void ASprungWheel::BeginPlay()
{
	Super::BeginPlay();
	
	// Enable collision and register ASprungWheel::OnHit with our wheel becoming hit.
	Wheel->SetNotifyRigidBodyCollision(true);
	Wheel->OnComponentHit.AddDynamic(this, &ASprungWheel::OnHit);

	SetupConstraint();

}

void ASprungWheel::SetupConstraint()
{
	if (!GetAttachParentActor())
	{
		return;

	}
	UPrimitiveComponent* BodyRoot = Cast<UPrimitiveComponent>(GetAttachParentActor()->GetRootComponent());
	if (!BodyRoot)
	{
		return;

	}

	// Specify components to constrain. 
	MassWheelConstraint->SetConstrainedComponents(BodyRoot, NAME_None, Axle, NAME_None);
	AxleWheelConstraint->SetConstrainedComponents(Axle, NAME_None, Wheel, NAME_None);

}

void ASprungWheel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TickGroup == TG_PostPhysics)
	{
		// Reset TotalForceMagnitudeThisFrame.
		TotalForceMagnitudeThisFrame = 0.f;

	}

}

void ASprungWheel::AddDrivingForce(float ForceMagnitude)
{
	// Calculate magnitude per frame.
	TotalForceMagnitudeThisFrame += ForceMagnitude;

}

void ASprungWheel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ApplyForce();

}

void ASprungWheel::ApplyForce()
{
	// Drive the wheels by multiplying the direction of the force by its magnitude.
	Wheel->AddForce(Axle->GetForwardVector() * TotalForceMagnitudeThisFrame);

}
