// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online/PBPacketManager.h"
#include "NetPacket.h"
#include "PBClientPacketManager.generated.h"

UCLASS()
class PBGAME_API UPBClientPacketManager : public UPBPacketManager
{
	GENERATED_BODY()
	
	bool SendEvent(S2MOPacketBase* pPacket) override;
	void OnDisconnected() override;

public: // Tests
	UFUNCTION(BlueprintCallable, Category = Messaging)
	void Test(int TestCode);
};
