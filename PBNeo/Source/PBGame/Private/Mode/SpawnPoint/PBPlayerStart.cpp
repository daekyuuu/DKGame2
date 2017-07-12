// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPlayerStart.h"

APBPlayerStart::APBPlayerStart()
{
	bShowOnMinimap = false;
	SpawnTeam = EPBTeamType::Max;
}

void APBPlayerStart::BeginPlay()
{
	Super::BeginPlay();
}

bool APBPlayerStart::IsAllowed(AController* Player)
{
	APBPlayerController* PlayerController = Cast<APBPlayerController>(Player);
	return PlayerController != nullptr;
}
