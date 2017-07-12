// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWaitingRoomPage.h"
#include "PBGameInstance.h"
#include "PBGameTableManager.h"
#include "PBMapTable.h"
#include "Online/PBRoomUserInfoManager.h"

FPBWaitingRoomInfo UPBWaitingRoomPage::GetWaitingRoomInfo()
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	UPBMapTable* MapTable = Cast<UPBMapTable>(UPBGameTableManager::Get(GetOwningLocalPlayer())->GetTable(EPBTableType::GameMap));
	ensure(ClientInterface && MapTable);

	FPBWaitingRoomInfo Rtn;
	Rtn.RoomMode = GetGameModeString(ClientInterface->GetCurrentRoomInfo().GameMode);
	Rtn.OwningPlayerTeamType = ClientInterface->GetOwningPlayerTeam();
	Rtn.SecondsTillStart = ClientInterface->GetCurrentRoomVoteBoardInfo().SecondsTillGameStart;

	FMapTableDataList* ModeMaps = MapTable->GetDataByMode(Rtn.RoomMode);
	if (ModeMaps)
	{
		for (auto& Elem : *ModeMaps)
		{
			FPBWaitingRoomMapVoteSlot MapSlotToAdd;
			MapSlotToAdd.MapIdx = Elem.Key;
			MapSlotToAdd.MapName = Elem.Value->Name;
			MapSlotToAdd.MapTexture = Elem.Value->IconResVote.Get();
			if (!MapSlotToAdd.MapTexture)
			{
				MapSlotToAdd.MapTexture = Elem.Value->IconResVote.LoadSynchronous();
			}

			uint8* VoteNumPtr = ClientInterface->GetCurrentRoomVoteBoardInfo().MapIdxToVoteCount.Find(Elem.Key);
			if (VoteNumPtr)
			{
				MapSlotToAdd.VoteNumber = *VoteNumPtr;
			}
			else
			{
				MapSlotToAdd.VoteNumber = 0;
			}

			Rtn.MapVoteSlots.Add(MapSlotToAdd);
		}
	}

	UPBRoomUserInfoManager* RoomUserManager = UPBRoomUserInfoManager::Get(GetOwningLocalPlayer());
	if (RoomUserManager)
	{
		for (auto& TeamSlots : RoomUserManager->GetUserSlots())
		{
			for (auto& Slot : TeamSlots.Value)
			{
				if (Slot.Value.SlotState == EPBRoomSlotState::Close || Slot.Value.SlotState == EPBRoomSlotState::Open)
				{
					// The slot has no player
					continue;
				}

				FPBWaitingRoomPlayerSlot PlayerSlotToAdd;
				PlayerSlotToAdd.TeamType = TeamSlots.Key;
				PlayerSlotToAdd.SlotIndex = Slot.Key;
				PlayerSlotToAdd.PlayerName = Slot.Value.UserBaseInfo.NickName;
				PlayerSlotToAdd.PlayerLevel = Slot.Value.UserBaseInfo.Level;

				if (ClientInterface->GetOwningPlayerTeam() == TeamSlots.Key)
				{
					if (ClientInterface->GetOwningPlayerSlotIdx() == Slot.Key)
					{
						PlayerSlotToAdd.Identity = EWaitingRoomPlayerSlotIdentity::Self;
					}
					else
					{
						PlayerSlotToAdd.Identity = EWaitingRoomPlayerSlotIdentity::Ally;
					}
				}
				else
				{
					PlayerSlotToAdd.Identity = EWaitingRoomPlayerSlotIdentity::Foe;
				}

				Rtn.PlayerSlots.Add(PlayerSlotToAdd);
			}
		}
	}
	
	return Rtn;
}

void UPBWaitingRoomPage::ReadyToPlay()
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->DeclareSelfReady();
}

void UPBWaitingRoomPage::VoteMap(uint32 MapIedx)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	if (!VotedForMapAlready)
	{
		ClientInterface->VoteMap(MapIedx);
		VotedForMapAlready = true;
	}
}

void UPBWaitingRoomPage::Enter()
{
	Super::Enter();

	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ensure(GetOwningLocalPlayer() && GetOwningLocalPlayer()->GetWorld());
	VotedForMapAlready = false;
	WaitingRoomRefreshDelegate.Broadcast();
	LastWaitingRoomRefreshTime = GetOwningLocalPlayer()->GetWorld()->RealTimeSeconds;
	HandleWaitingRoomDataUpdated_Handle = ClientInterface->OnCurrentRoomDataUpdatedEvt.AddUObject(this, &UPBWaitingRoomPage::HandleWaitingRoomDataUpdated);
	HandleBattleStart_Handle = ClientInterface->OnBattleStartEvt.AddUObject(this, &UPBWaitingRoomPage::HandleBattleStart);

	//for Debug Cheat
	UPBRoomUserInfoManager* RoomUserManager = UPBRoomUserInfoManager::Get(GetOwningLocalPlayer());
	ensure(RoomUserManager);
	RoomUserManager->OnCurrentRoomDataUpdatedEvt.AddUObject(this, &UPBWaitingRoomPage::HandleWaitingRoomDataUpdated);
}

void UPBWaitingRoomPage::HandleWaitingRoomDataUpdated()
{
	static const float WaitingRoomRefreshPeriod = 0.2;

	if (GetOwningLocalPlayer() && GetOwningLocalPlayer()->GetWorld())
	{
		float CurrentTime = GetOwningLocalPlayer()->GetWorld()->RealTimeSeconds;
		if (CurrentTime - LastWaitingRoomRefreshTime > WaitingRoomRefreshPeriod)
		{
			// Avoids flickering of the waiting room UI display when two data packet come at same time
			WaitingRoomRefreshDelegate.Broadcast();
			LastWaitingRoomRefreshTime = CurrentTime;
		}
	}
}

void UPBWaitingRoomPage::HandleBattleStart(TArray<FPBRoomSlotInfo> StartingPlayers, EPBTeamType OwningPlayerTeam, uint32 MapIdx, FString DediAddr)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->TravelToGame(StartingPlayers, OwningPlayerTeam,  MapIdx, DediAddr);
}

void UPBWaitingRoomPage::Exit()
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(GetOwningLocalPlayer());
	ensure(ClientInterface);

	ClientInterface->OnBattleStartEvt.Remove(HandleBattleStart_Handle);
	ClientInterface->OnCurrentRoomDataUpdatedEvt.Remove(HandleWaitingRoomDataUpdated_Handle);
	ClientInterface->LeaveCurrentRoom();

	Super::Exit();
}
