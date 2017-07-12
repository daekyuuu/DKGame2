// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBCharacterMovement.h"

//----------------------------------------------------------------------//
// UPawnMovementComponent
//----------------------------------------------------------------------//
UPBCharacterMovement::UPBCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


float UPBCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const APBCharacter* PBCharacterOwner = Cast<APBCharacter>(PawnOwner);
	if (PBCharacterOwner)
	{
		MaxSpeed = PBCharacterOwner->ApplyMovementSpeedModifier(MaxSpeed);
	}

	return MaxSpeed;
}
