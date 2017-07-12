// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online/PBPacketManager.h"
#include "GameServerNetPacket.h"
#include "PBServerPacketManager.generated.h"

UCLASS()
class PBGAME_API UPBServerPacketManager : public UPBPacketManager
{
	GENERATED_BODY()
	
	bool SendEvent(S2MOPacketBase* pPacket) override;
	void OnDisconnected() override;
	
};
