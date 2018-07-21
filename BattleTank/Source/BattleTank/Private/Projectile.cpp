// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/RadialForceComponent.h"
#include "Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


AProjectile::AProjectile()
{

	PrimaryActorTick.bCanEverTick = false;

	// Create a UStaticMeshComponent, enable collision, and set invisible. 
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(false);

	// Create a UProjectileMovementComponent to give the projectile access to bullet logic.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;

	// Create a UParticleSystemComponent to add VFX.
	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Launch Blast"));
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform); // TODO update to new API

	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact Blast"));
	ImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactBlast->bAutoActivate = false;

	// Create a URadialForceComponent to add a force to each explosion.
	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Register the AProjectile::OnHit method with our collision mesh becoming hit.
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	
}

void AProjectile::LaunchProjectile(float Speed)
{
	// Set velocity by multiplying the projectile's direction with its speed.
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Deactivate launch particles and play impact particles.
	LaunchBlast->Deactivate();
	ImpactBlast->Activate();

	// Create radial force at point of impact.
	ExplosionForce->FireImpulse();

	// Replace the root with the impact particles so our mesh can be destroyed without stopping the VFX.
	SetRootComponent(ImpactBlast);
	CollisionMesh->DestroyComponent();

	/**
	 * Apply damage in a radius around impact.
	 * @param1 what obj is applying the damage
	 * @param2 base damage to apply
	 * @param3 epicenter of radial damage
	 * @param4 radius of radial damage
	 * @param5 no special damage type
	 * @param6 what can take damage
	 *
	 */
	UGameplayStatics::ApplyRadialDamage(
		this,
		ProjectileDamage,
		GetActorLocation(),
		ExplosionForce->Radius, // for consistancy
		UDamageType::StaticClass(),
		TArray<AActor*>() // damage all actors
	);

	// Wait to destroy projectile to allow VFX and damage to play through.
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpire, DestroyDelay, false);

}

void AProjectile::OnTimerExpire()
{
	Destroy();

}
