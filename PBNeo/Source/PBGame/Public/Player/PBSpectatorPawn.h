// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "PBSpectatorPawn.generated.h"


UCLASS(config = Game, Blueprintable, BlueprintType)
class APBSpectatorPawn : public ASpectatorPawn
{
	GENERATED_UCLASS_BODY()

	/** Overridden to implement Key Bindings the match the player controls */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Frame rate linked look
	void LookUpAtRate(float Val);
};
