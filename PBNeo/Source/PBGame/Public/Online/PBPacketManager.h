// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Networking.h"
#include "S2MOPacket.h"
#include "PBPacketManager.generated.h"

struct FPacketRecvEvent
{
	T_PACKET_PROTOCOL		TProtocol;
	char									Buffer[ S2MO_PACKET_CONTENT_SIZE ];
};

UCLASS()
class PBGAME_API UPBPacketManager : public UObject
{
	GENERATED_BODY()

public: // To be called by owning manager

	static UPBPacketManager* Get(UObject* WorldContextObject);

	void Init();

	void Update();

	void ShutDown();

	virtual bool SendEvent(S2MOPacketBase* pPacket);

	DECLARE_EVENT_OneParam(UPBPacketManager, FOnRecvEvent, FPacketRecvEvent /* Event */);
	FOnRecvEvent OnRecvEvent;

protected: // Implementation Details

	bool ConnectTo(const FString& IP, const int32 Port);
	virtual void OnDisconnected();

	void PacketParsing(char* Buffer);

protected: // Fields

	FSocket* ConnectionSocket;

	int32 CurrentSendOffset = 0;
	TQueue<TSharedPtr<TArray<uint8>>, EQueueMode::Spsc> SendBuffers;

	char RecvBuffer[S2MO_PACKET_CONTENT_SIZE];

public: // Util

	template<int N>
	static void UnrealStringToS2MOString(const FString& Input, S2MOStringW<N>& Output);

	template<int N>
	static void S2MOStringToUnrealString(S2MOStringW<N>& Input, FString& Output);
};

template<int N>
void UPBPacketManager::UnrealStringToS2MOString(const FString& Input, S2MOStringW<N>& Output)
{
	if (Input.IsEmpty())
	{
		Output = S2MOStringW<N>();
	}
	else
	{
		Output = (CHAR16*)(StringCast<UCS2CHAR>(Input.GetCharArray().GetData(), Input.GetCharArray().Num()).Get());
	}
}

template<int N>
void UPBPacketManager::S2MOStringToUnrealString(S2MOStringW<N>& Input, FString& Output)
{
	CHAR16 Str[N];
	Input.GetValue(Str, N);
	Output = FString(StringCast<TCHAR>((UCS2CHAR*)Str).Get());
}
