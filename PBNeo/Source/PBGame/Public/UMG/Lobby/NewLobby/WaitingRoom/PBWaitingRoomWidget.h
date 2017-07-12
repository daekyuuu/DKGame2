// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/PBLobbyWidget.h"
#include "PBWaitingRoomPage.h"
#include "PBWaitingRoomWidget.generated.h"

UCLASS()
class PBGAME_API UPBWaitingRoomWidget : public UPBLobbyWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "WaitingRoom")
	FPBWaitingRoomInfo GetWaitingRoomInfo();

	UFUNCTION(BlueprintCallable, Category = "WaitingRoom")
	void OnReadyButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "WaitingRoom")
	void OnMapVoted(int MapIdx);
};
