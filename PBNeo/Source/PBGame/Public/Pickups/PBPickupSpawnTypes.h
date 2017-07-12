// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PBPickupSpawnTypes.generated.h"

USTRUCT()
struct FPickupSpawnChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	TSubclassOf<APBPickup> ItemType;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	float Chance;

	FPickupSpawnChance()
	{
		ItemType = nullptr;
		Chance = 100.f;
	}
};

USTRUCT()
struct FPickupRespawnElem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	float InitialSpawnDelay;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	float NextSpawnDelay;

	// if value is -1, infinity
	UPROPERTY(EditAnywhere, Category = SpawnInfo, meta = (DisplayName = "MaxSpawnNum (-1 == infinity)"))
	int32 MaxSpawnNum;

	int32 SpawnNum;
	FTimerHandle TimerHandle_Respawn;

	
	// 0 mean don't use destroyDelay
	float DestroyDelay;
	FTimerHandle TimerHandle_Destroy;

	bool CanSpawn();

	FPickupRespawnElem()
	{
		InitialSpawnDelay = 0.1f;
		NextSpawnDelay = 5.f;
		MaxSpawnNum = -1;
		SpawnNum = 0;
		DestroyDelay = 0.0f;
	}
};

UENUM(BlueprintType)
enum class EPickupSpawnRule : uint8
{
	Random,
	Cycle,
};
