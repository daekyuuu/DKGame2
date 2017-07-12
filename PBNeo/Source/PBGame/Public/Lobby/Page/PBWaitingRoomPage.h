// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Lobby/Page/PBWidgetPage.h"
#include "PBNetClientInterface.h"
#include "PBWaitingRoomPage.generated.h"

USTRUCT(BlueprintType)
struct FPBWaitingRoomMapVoteSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int MapIdx;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString MapName;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	UTexture2D* MapTexture;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int VoteNumber;
};

UENUM(BlueprintType)
enum class EWaitingRoomPlayerSlotIdentity : uint8
{
	Self,
	Ally,
	Foe,
};

USTRUCT(BlueprintType)
struct FPBWaitingRoomPlayerSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	EPBTeamType TeamType;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int SlotIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	EWaitingRoomPlayerSlotIdentity Identity;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int PlayerLevel;
};

USTRUCT(BlueprintType)
struct FPBWaitingRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString RoomMode;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	EPBTeamType OwningPlayerTeamType;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int SecondsTillStart;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	TArray<FPBWaitingRoomMapVoteSlot> MapVoteSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	TArray<FPBWaitingRoomPlayerSlot> PlayerSlots;
};

UCLASS()
class PBGAME_API UPBWaitingRoomPage : public UPBWidgetPage
{
	GENERATED_BODY()

	/* ------------------------------------------------------------------------------- */
	//  Delegates
	/* ------------------------------------------------------------------------------- */

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitingRoomUpdatedDelegate);
	UPROPERTY(BlueprintAssignable, Category = "WaitingRoom")
	FWaitingRoomUpdatedDelegate WaitingRoomRefreshDelegate;
	
	/* ------------------------------------------------------------------------------- */
	//  Functions will be called by UI event.
	/* ------------------------------------------------------------------------------- */

public:

	// Get the waiting room data and convert it into a format useful for the Waiting Room UI
	FPBWaitingRoomInfo GetWaitingRoomInfo();
	
	void ReadyToPlay();

	void VoteMap(uint32 MapIedx);

	/* ------------------------------------------------------------------------------- */
	//  Main
	/* ------------------------------------------------------------------------------- */

	void Enter() override;
	bool VotedForMapAlready;
	float LastWaitingRoomRefreshTime;
	FDelegateHandle HandleWaitingRoomDataUpdated_Handle;
	void HandleWaitingRoomDataUpdated();
	FDelegateHandle HandleBattleStart_Handle;
	void HandleBattleStart(TArray<FPBRoomSlotInfo> StartingPlayers, EPBTeamType OwningPlayerTeam, uint32 MapIdx, FString DediAddr);
	void Exit() override;

	/* ------------------------------------------------------------------------------- */
	//  End
	/* ------------------------------------------------------------------------------- */
};
