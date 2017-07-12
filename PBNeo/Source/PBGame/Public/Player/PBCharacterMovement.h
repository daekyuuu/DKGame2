// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/**
 * Movement component meant for use with Pawns.
 */

#pragma once

#include "Player/PBCharacterTypes.h"
#include "PBCharacterMovement.generated.h"

UCLASS()
class UPBCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual float GetMaxSpeed() const override;
};

