// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionQuickThrow_Knife.h"




UPBWeapActionQuickThrow_Knife::UPBWeapActionQuickThrow_Knife()
{
	bThrowActivated = false;
}

void UPBWeapActionQuickThrow_Knife::StartThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		// Todo
		//APBWeap_Throw* ThrowWeapon = Cast<APBWeap_Throw>(MyPawn->GetKnifeWeapon());
		//bThrowActivated = MyPawn->StartQuickThrow(ThrowWeapon);
	}
}

void UPBWeapActionQuickThrow_Knife::EndThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		if (bThrowActivated)
		{
			APBWeap_Throw* ThrowWeapon = Cast<APBWeap_Throw>(MyPawn->GetCurrentQuickWeapon());
			MyPawn->StopQuickThrow(ThrowWeapon);
			bThrowActivated = false;
		}
	}
}
