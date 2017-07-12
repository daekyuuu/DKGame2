// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPacketManager.h"

UPBPacketManager* UPBPacketManager::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetPacketManager(WorldContextObject);
}

void UPBPacketManager::Init()
{

}

bool UPBPacketManager::ConnectTo(const FString& IP, const int32 Port)
{
	TSharedPtr<FInternetAddr> TargetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	if (!TargetAddr.IsValid())
	{
		return false;
	}

	bool bIsValid;
	TargetAddr->SetIp(*IP, bIsValid);
	TargetAddr->SetPort(Port);
	if (!bIsValid)
	{
		return false;
	}

	// Reconnect if already connected - we first close the old connection
	if (ConnectionSocket)
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
	}

	int32 SendSize = 2 * 1024 * 1024;
	int32 RecvSize = 2 * 1024 * 1024;
	ConnectionSocket = FTcpSocketBuilder(TEXT("MessageSendingSocket"))
		.AsReusable().AsNonBlocking()
		.WithSendBufferSize(SendSize)
		.WithReceiveBufferSize(RecvSize);
	if (!ConnectionSocket)
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
		return false;
	}

	if (!ConnectionSocket->Connect(*TargetAddr))
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
		return false;
	}

	return true;
}

void UPBPacketManager::OnDisconnected()
{
	// notify event for subclass
}

bool UPBPacketManager::SendEvent( S2MOPacketBase* pPacket )
{
	if (!ConnectionSocket)
	{
		return false;
	}

	char pBuffer[S2MO_PACKET_CONTENT_SIZE];
	pPacket->Packing_T(pBuffer);

	TSharedPtr<TArray<uint8>> DataToSend = MakeShareable(new TArray<uint8>());
	if (!DataToSend.IsValid())
		return false;

	DataToSend->AddDefaulted(pPacket->GetPacketSize());
	FMemory::Memcpy(DataToSend->GetData(), pBuffer, pPacket->GetPacketSize());
	return SendBuffers.Enqueue(DataToSend);
}

void UPBPacketManager::Update()
{
	if (ConnectionSocket == nullptr)
	{
		return;
	}
	else if (SCS_Connected != ConnectionSocket->GetConnectionState())
	{
		// (ConnectionSocket != nullptr) means socket was once connected and user still think socket is connected
		// However now we found that ConnectionSocket is not connected - this is disconnection event
		OnDisconnected();

		// Make sure socket is cleaned up
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;

		return;
	}
	
	// Try send one message
	if (!SendBuffers.IsEmpty())
	{
		TSharedPtr<TArray<uint8>> DataToSend;
		SendBuffers.Peek(DataToSend);

		if (DataToSend.IsValid())
		{
			int32 ByteSent;
			ConnectionSocket->Send(DataToSend->GetData() + CurrentSendOffset, DataToSend->Num() - CurrentSendOffset, ByteSent);
			CurrentSendOffset += ByteSent;

			if (CurrentSendOffset == DataToSend->Num())
			{
				SendBuffers.Dequeue(DataToSend);
				CurrentSendOffset = 0;
			}
		}
		else
		{
			// Discard invalid data
			SendBuffers.Dequeue(DataToSend);
			CurrentSendOffset = 0;
		}
	}

	//  Try receive one message
	uint32 BytesLeftToProcess = 0;
	if (ConnectionSocket->HasPendingData(BytesLeftToProcess) && BytesLeftToProcess >= sizeof(T_PACKET_SIZE))
	{
		T_PACKET_SIZE MessageSize;

		int32 BytesPeeked = 0;
		if (ConnectionSocket->Recv((uint8*)&MessageSize, sizeof(T_PACKET_SIZE), BytesPeeked, ESocketReceiveFlags::Peek) && BytesPeeked == sizeof(T_PACKET_SIZE))
		{
			if (BytesLeftToProcess >= (uint32)MessageSize)
			{
				int32 BytesRead = 0;			
				ConnectionSocket->Recv((uint8*)RecvBuffer, MessageSize, BytesRead, ESocketReceiveFlags::None);
				ensure(BytesRead == MessageSize);
				PacketParsing(RecvBuffer);
			}
		}
	}
}

void UPBPacketManager::PacketParsing( char* Buffer )
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( Buffer );

	FPacketRecvEvent Event;
	Event.TProtocol = PacketBase.GetProtocol();
	memcpy( Event.Buffer, Buffer, PacketBase.GetPacketSize() );
	OnRecvEvent.Broadcast(Event);
}

void UPBPacketManager::ShutDown()
{
	if (ConnectionSocket)
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
	}
}