// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBInGameSpectatorPawn.h"




APBInGameSpectatorPawn::APBInGameSpectatorPawn()
{
	bFindCameraComponentWhenViewTarget = false;
}

void APBInGameSpectatorPawn::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location.Z += BaseEyeHeight;
	UE_LOG(LogStats, Display, TEXT("EyesViewPoint InGameSpectatorPaw %s"), *FString::Printf(TEXT("cam Loc(%f: %f: %f) Rot(%f: %f: %f)"), Location.X, Location.Y, Location.Z, Rotation.Yaw, Rotation.Pitch, Rotation.Roll));
}
