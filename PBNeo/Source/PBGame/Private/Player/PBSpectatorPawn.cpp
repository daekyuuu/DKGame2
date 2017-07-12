// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "PBGame.h"
#include "Player/PBSpectatorPawn.h"

APBSpectatorPawn::APBSpectatorPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{	
}

void APBSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ADefaultPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADefaultPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ADefaultPawn::MoveUp_World);
	PlayerInputComponent->BindAxis("Turn", this, &ADefaultPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADefaultPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ADefaultPawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APBSpectatorPawn::LookUpAtRate);
}

void APBSpectatorPawn::LookUpAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	const float DeltaSeconds = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;
	AddControllerPitchInput(Val * BaseLookUpRate * DeltaSeconds * CustomTimeDilation);
}
