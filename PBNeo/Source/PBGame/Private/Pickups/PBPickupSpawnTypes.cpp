// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupSpawnTypes.h"


bool FPickupRespawnElem::CanSpawn()
{
	// if MaxSpawnNum == -1, infinity
	if (MaxSpawnNum == 0 || (MaxSpawnNum > 0 && SpawnNum >= MaxSpawnNum))
	{
		return false;
	}

	// check respawn Timer (First time unconditional spawn)
	if (SpawnNum > 0 && NextSpawnDelay <= 0.f)
	{
		return false;
	}

	return true;
}
