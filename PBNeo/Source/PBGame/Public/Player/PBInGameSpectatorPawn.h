// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/PBSpectatorPawn.h"
#include "PBInGameSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBInGameSpectatorPawn : public APBSpectatorPawn
{
	GENERATED_BODY()
	
public:	
	APBInGameSpectatorPawn();

	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	
};
