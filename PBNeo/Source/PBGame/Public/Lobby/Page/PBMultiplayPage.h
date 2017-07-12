// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Lobby/Page/PBWidgetPage.h"
#include "PBMultiplayPage.generated.h"

USTRUCT(BlueprintType)
struct FPBRoomEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString RoomTitle;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString GameMode;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int CurrentPlayers;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	int MaxPlayers;

	UPROPERTY()
	int RoomIdx;
};

UCLASS()
class UPBRoomEntryReader : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = ServerEntry)
	static FString GetRoomTitle(const FPBRoomEntry& Value)
	{
		return Value.RoomTitle;
	}

	UFUNCTION(BlueprintCallable, Category = ServerEntry)
	static FString GetGameMode(const FPBRoomEntry& Value)
	{
		return Value.GameMode;
	}

	UFUNCTION(BlueprintCallable, Category = ServerEntry)
	static int GetCurrentPlayers(const FPBRoomEntry& Value)
	{
		return Value.CurrentPlayers;
	}

	UFUNCTION(BlueprintCallable, Category = ServerEntry)
	static int GetMaxPlayers(const FPBRoomEntry& Value)
	{
		return Value.MaxPlayers;
	}
};

/**
 * 
 */
UCLASS(config = Game)
class PBGAME_API UPBMultiplayPage : public UPBWidgetPage
{
	GENERATED_BODY()
public:

	UPBMultiplayPage();

	/* ------------------------------------------------------------------------------- */
	//  Delegates
	/* ------------------------------------------------------------------------------- */

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSearchServerDoneDelegate, bool, bIsSuccess, const TArray<FPBRoomEntry>&, RoomList);
	UPROPERTY(BlueprintAssignable, Category = "Multiplay")
	FSearchServerDoneDelegate ServerSearchDoneDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostGameDoneDelegate, bool, bIsSuccess);
	UPROPERTY(BlueprintAssignable, Category = "Multiplay")
	FHostGameDoneDelegate HostGameDoneDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinGameDoneDelegate, bool, bIsSuccess);
	UPROPERTY(BlueprintAssignable, Category = "Multiplay")
	FJoinGameDoneDelegate JoinGameDoneDelegate;

	/* ------------------------------------------------------------------------------- */
	//  Functions will be called by UI event.
	/* ------------------------------------------------------------------------------- */

public:

	void OnModeChanged(int32 Index);
	void OnMemberChanged(int32 Index);
	void OnTimeChanged(int32 Index);
	void OnRoundChanged(int32 Index);
	EPBAsyncResult OnCreateRoom();
	EPBAsyncResult OnSearchServerToJoin();
	EPBAsyncResult OnConnectToServer(const FPBRoomEntry& RoomEntry);

	/* ------------------------------------------------------------------------------- */
	//  Main
	/* ------------------------------------------------------------------------------- */

private:

	EPBAsyncResult HostGame();
	void OnUserCanPlayOnlineHost(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	FDelegateHandle EnterHostedWaitingRoom_Handle;
	void EnterHostedWaitingRoom(bool Success);

	EPBAsyncResult FindGame();
	void OnUserCanPlayOnlineJoin(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	FDelegateHandle UpdateSearchStatus_Handle;
	void UpdateSearchStatus(TArray<struct FPBRoomInfo> Info);

	EPBAsyncResult JoinGame(const FPBRoomEntry& RoomEntry);
	FDelegateHandle EnterJoinedWaitingRoom_Handle;
	void EnterJoinedWaitingRoom(bool Success);

private:
	int32 SelectedGameModeIndex;

	/* ------------------------------------------------------------------------------- */
	// Helper for online privilege validation
	/* ------------------------------------------------------------------------------- */

private:

	bool ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer);

	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);

	/* ------------------------------------------------------------------------------- */
	//  End
	/* ------------------------------------------------------------------------------- */
};
