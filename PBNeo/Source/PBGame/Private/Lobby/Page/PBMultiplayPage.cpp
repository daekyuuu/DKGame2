// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMultiplayPage.h"
#include "PBGameInstance.h"
#include "GenericPlatformChunkInstall.h"
#include "PBGameSession.h"
#include "PBPersistentUser.h"
#include "PBLobby.h"
#include "PBWaitingRoomPage.h"
#include "PBGameplayStatics.h"
#include "PBNetClientInterface.h"

UPBMultiplayPage::UPBMultiplayPage()
{
	SelectedGameModeIndex = 0;
}

void UPBMultiplayPage::OnModeChanged(int32 Index)
{
	SelectedGameModeIndex = Index;
}

void UPBMultiplayPage::OnMemberChanged(int32 Index)
{
}

void UPBMultiplayPage::OnTimeChanged(int32 Index)
{
}

void UPBMultiplayPage::OnRoundChanged(int32 Index)
{
}

EPBAsyncResult UPBMultiplayPage::OnCreateRoom()
{
	return HostGame();
}

EPBAsyncResult UPBMultiplayPage::OnSearchServerToJoin()
{
	return FindGame();
}

EPBAsyncResult UPBMultiplayPage::OnConnectToServer(const FPBRoomEntry& RoomEntry)
{
	return JoinGame(RoomEntry);
}

EPBAsyncResult UPBMultiplayPage::HostGame()
{
#if PB_CONSOLE_UI
	if (!GetOwningLocalPlayer()
		||
		!ValidatePlayerForOnlinePlay(GetOwningLocalPlayer()))
	{
		return EPBAsyncResult::Failure;
	}
#endif

	StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UPBMultiplayPage::OnUserCanPlayOnlineHost));
	return EPBAsyncResult::Pending;
}

void UPBMultiplayPage::OnUserCanPlayOnlineHost(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	GetGameInstance()->CleanupOnlinePrivilegeTask();

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
		ensure(ClientInterface);

		ClientInterface->CreateRoom(FString("GameRoom"), (EPBGameModeType)SelectedGameModeIndex, 180, 8);
		EnterHostedWaitingRoom_Handle = ClientInterface->OnEnterRoomEvt.AddUObject(this, &UPBMultiplayPage::EnterHostedWaitingRoom);
	}
	else
	{
		HostGameDoneDelegate.Broadcast(false);
		GetGameInstance()->DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
	}
}

void UPBMultiplayPage::EnterHostedWaitingRoom(bool Success)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->OnEnterRoomEvt.Remove(EnterHostedWaitingRoom_Handle);

	if (Success)
	{
		HostGameDoneDelegate.Broadcast(true);

		UPBWaitingRoomPage* WaitingRoom = Cast<UPBWaitingRoomPage>(GetGameInstance()->LobbyInstance->GetPageInstance(EPageType::WaitingRoom));
		GetGameInstance()->LobbyInstance->PushPage(EPageType::WaitingRoom);
	}
	else
	{
		HostGameDoneDelegate.Broadcast(false);
	}
}

EPBAsyncResult UPBMultiplayPage::FindGame()
{
#if PB_CONSOLE_UI
	if (!GetOwningLocalPlayer()
		||
		!ValidatePlayerForOnlinePlay(GetOwningLocalPlayer()))
	{
		return EPBAsyncResult::Failure;
	}
#endif

	StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UPBMultiplayPage::OnUserCanPlayOnlineJoin));
	return EPBAsyncResult::Pending;
}

void UPBMultiplayPage::OnUserCanPlayOnlineJoin(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	GetGameInstance()->CleanupOnlinePrivilegeTask();

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
		ensure(ClientInterface);

		ClientInterface->FindRoom();
		UpdateSearchStatus_Handle = ClientInterface->OnRoomListEvt.AddUObject(this, &UPBMultiplayPage::UpdateSearchStatus);
	}
	else
	{
		// Privilege Failure
		ServerSearchDoneDelegate.Broadcast(false, TArray<FPBRoomEntry>());
		GetGameInstance()->DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
	}
}

void UPBMultiplayPage::UpdateSearchStatus(TArray<FPBRoomInfo> Info)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);
	
	ClientInterface->OnRoomListEvt.Remove(UpdateSearchStatus_Handle);

	TArray<FPBRoomEntry> RoomList;
	for (int32 Idx = 0; Idx < Info.Num(); ++Idx)
	{
		if (Info[Idx].CurrentUserNum < Info[Idx].MaxUserNum)
		{
			RoomList.AddDefaulted(1);
			RoomList.Last().RoomTitle = Info[Idx].Title;
			RoomList.Last().GameMode = GetGameModeString(Info[Idx].GameMode);
			RoomList.Last().CurrentPlayers = Info[Idx].CurrentUserNum;
			RoomList.Last().MaxPlayers = Info[Idx].MaxUserNum;
			RoomList.Last().RoomIdx = Info[Idx].RoomIdx;
		}
	}

	ServerSearchDoneDelegate.Broadcast(true, RoomList);
}

EPBAsyncResult UPBMultiplayPage::JoinGame(const FPBRoomEntry& RoomEntry)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->EnterRoom(RoomEntry.RoomIdx);
	EnterJoinedWaitingRoom_Handle = ClientInterface->OnEnterRoomEvt.AddUObject(this, &UPBMultiplayPage::EnterJoinedWaitingRoom);
	return EPBAsyncResult::Pending;
}

void UPBMultiplayPage::EnterJoinedWaitingRoom(bool Success)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->OnEnterRoomEvt.Remove(EnterJoinedWaitingRoom_Handle);

	if (Success)
	{
		JoinGameDoneDelegate.Broadcast(true);

		UPBWaitingRoomPage* WaitingRoom = Cast<UPBWaitingRoomPage>(GetGameInstance()->LobbyInstance->GetPageInstance(EPageType::WaitingRoom));
		GetGameInstance()->LobbyInstance->PushPage(EPageType::WaitingRoom);
	}
	else
	{
		JoinGameDoneDelegate.Broadcast(false);
	}
}

bool UPBMultiplayPage::ValidatePlayerForOnlinePlay(ULocalPlayer* InLocalPlayer)
{
	if (!ensure(GetGameInstance()))
	{
		return false;
	}

	return GetGameInstance()->ValidatePlayerForOnlinePlay(InLocalPlayer);
}

void UPBMultiplayPage::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	ensure(GetGameInstance() && GetOwningLocalPlayer());

	TSharedPtr<const FUniqueNetId> UserId;
	UserId = GetOwningLocalPlayer()->GetPreferredUniqueNetId();
	GetGameInstance()->StartOnlinePrivilegeTask(Delegate, EUserPrivileges::CanPlayOnline, UserId);
}