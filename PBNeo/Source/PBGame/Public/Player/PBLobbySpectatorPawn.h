// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "PBLobbySpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBLobbySpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	APBLobbySpectatorPawn();

	class UCameraComponent* GetCameraComponent() const;

public:

	UPROPERTY()
		class UCameraComponent* CameraComp;

	
	
};
