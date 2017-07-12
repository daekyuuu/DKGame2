// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UObject/NoExportTypes.h"
#include "Object.h"
#include "PBNetServerInterface.generated.h"

enum class EPBNetServerState : uint8
{
	NotConnected,
	Connected,
	LoggedIn,
};

class UGameInstance;

UCLASS(Within = GameInstance)
class PBGAME_API UPBNetServerInterface : public UObject
{
	GENERATED_BODY()
	
public:

	static UPBNetServerInterface* Get(UObject* WorldContextObject);
	
	void Init();
	void ShutDown();

	EPBNetServerState GetState();
	bool ConnectToServer(FString IP, uint16 port);
	bool Login(uint16 myListenPort, int32 RoomIdx);
	bool AckLoadMapSignal(bool Success);
	bool AckShutdownSignal();

	DECLARE_EVENT_TwoParams(UPBNetClientInterface, FOnStateChangedEvt, EPBNetServerState, EPBNetServerState)
	FOnStateChangedEvt OnStateChangedEvt;

	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnLoadMapSignalEvt, int32)
	FOnLoadMapSignalEvt OnLoadMapSignalEvt;

	DECLARE_EVENT(UPBNetClientInterface, FOnShutdownSignalEvt)
	FOnShutdownSignalEvt OnShutdownSignalEvt;

	DECLARE_EVENT(UPBNetClientInterface, FOnKillSignalEvt)
	FOnKillSignalEvt OnKillSignalEvt;

protected:

	FDelegateHandle HandleRecvEvent_Handle;
	void HandleRecvEvent(struct FPacketRecvEvent RecvEvent);

	EPBNetServerState ServerState;
};
