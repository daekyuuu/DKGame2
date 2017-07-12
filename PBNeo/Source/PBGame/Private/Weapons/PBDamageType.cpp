// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Weapons/PBDamageType.h"

UPBDamageType::UPBDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DeathImpulseOverNormalImpulse = 1.f;
	DeathJumpImpulseSize = 0.f;
}

UPBFallOffDamageType::UPBFallOffDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCausedByWorld = true;
}

UPBPointDamageType::UPBPointDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ImpulseMultiplier_ShootDir = 1.f;
	ImpulseMultiplier_HitDir = 0.f;
}

UPBRadialDamageType::UPBRadialDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}