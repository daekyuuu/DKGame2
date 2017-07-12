// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionZooming.h"
#include "Player/PBCharacter.h"



UPBWeapActionZooming::UPBWeapActionZooming()
{

}

void UPBWeapActionZooming::BeginState(EFireTrigger eFireTrigger)
{
	Super::BeginState(eFireTrigger);
	
	APBCharacter* Pawn = GetOwnerPawn();
	if (Pawn)
	{
		Pawn->OnZoomIn();
	}
}

void UPBWeapActionZooming::EndState(EFireTrigger eFireTrigger)
{
	Super::EndState(eFireTrigger);
	
	APBCharacter* Pawn = GetOwnerPawn();
	if (Pawn)
	{
		Pawn->OnZoomOut();
	}
}
