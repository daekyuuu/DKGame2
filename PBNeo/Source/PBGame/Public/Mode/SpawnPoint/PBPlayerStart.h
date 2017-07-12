// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBTypes.h"
#include "PBPlayerStart.generated.h"
/**
 * 
 */
UCLASS()
class PBGAME_API APBPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	APBPlayerStart();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool IsAllowed(AController* Player);

	UPROPERTY(EditAnywhere, Category = Minimap)
	bool bShowOnMinimap;

	/** Which team can start at this point */
	UPROPERTY(EditDefaultsOnly, Category = Team)
	EPBTeamType SpawnTeam;
};
