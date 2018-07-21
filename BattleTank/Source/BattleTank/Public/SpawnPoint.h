// Copyright SeverZmith

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpawnPoint.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLETANK_API USpawnPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	USpawnPoint();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Inline GetSpawnedActor
	AActor* GetSpawnedActor() const { return SpawnedActor; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY()
	AActor* SpawnedActor;
	
};
