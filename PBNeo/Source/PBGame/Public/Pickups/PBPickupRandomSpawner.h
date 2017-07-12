// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickups/PBPickupSpawnTypes.h"
#include "PBPickupRandomSpawner.generated.h"

class APBPickup;

//--------------------------------------------------------------------------------

UCLASS(Blueprintable, HideDropdown)
class APBPickupRandomSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBPickupRandomSpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;	

	void OnPickedUp(APBPickup* Pickup);

protected:

	virtual void Reset() override;
	void HandleSpawn();

	void OnSpawn(TSubclassOf<APBPickup> WantsType);
	bool CanSpawn(TSubclassOf<APBPickup> WantsType);

	TSubclassOf<APBPickup> GetSpawnTypeRandom();	
	TSubclassOf<APBPickup> GetSpawnTypeCycle();

protected:

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	TArray<FPickupSpawnChance> SpawnChances;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	FPickupRespawnElem RespawnElem;

	// Random or cycle
	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	EPickupSpawnRule SpawnRule;

private:
	// used for next spawn (or changed host)
	UPROPERTY(Transient, Replicated)
	TSubclassOf<APBPickup> SpawnedPickupItemType;

	APBPickup* SpawnedPickupItem;

	FTimerHandle TimerHandle_Respawn;
};
