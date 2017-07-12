// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbySpectatorPawn.h"





APBLobbySpectatorPawn::APBLobbySpectatorPawn()
{
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("LobbyCameraComp"));
	if (CameraComp)
	{
		CameraComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

class UCameraComponent* APBLobbySpectatorPawn::GetCameraComponent() const
{
	return CameraComp;
}
