// Copyright SeverZmith

#include "SpawnPoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


USpawnPoint::USpawnPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void USpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	/**
	 * SpawnActorDeferred method will not run the SpawnPoint's construction script so we can set values before-hand.
	 * NOTE: When using the deferred method, you must designate when the actor is finished spawning with the FinishSpawningActor method.
	 *
	 */
	SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnClass, GetComponentTransform());
	if (!SpawnedActor) { return; }
	SpawnedActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	UGameplayStatics::FinishSpawningActor(SpawnedActor, GetComponentTransform());

}


// Called every frame
void USpawnPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
