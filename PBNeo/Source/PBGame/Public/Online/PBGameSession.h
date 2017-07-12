// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"
#include "PBGameSession.generated.h"

struct FPBGameSessionParams
{
	/** Name of session settings are stored with */
	FName SessionName;
	/** LAN Match */
	bool bIsLAN;
	/** Presence enabled session */
	bool bIsPresence;
	/** Id of player initiating lobby */
	TSharedPtr<const FUniqueNetId> UserId;
	/** Current search result choice to join */
	int32 BestSessionIdx;

	FPBGameSessionParams()
		: SessionName(NAME_None)
		, bIsLAN(false)
		, bIsPresence(false)
		, BestSessionIdx(0)
	{
	}
};

UCLASS(config=Game)
class PBGAME_API APBGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

protected:

	/** Delegate for creating a new session */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/** Delegate after starting a session */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Transient properties of a session during game creation/matchmaking */
	FPBGameSessionParams CurrentSessionParams;
	/** Current host settings */
	TSharedPtr<class FPBOnlineSessionSettings> HostSettings;
	/** Current search settings */
	TSharedPtr<class FPBOnlineSearchSettings> SearchSettings;

	/**
	 * Delegate fired when a session create request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * Delegate fired when a session start request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnStartOnlineGameComplete(FName InSessionName, bool bWasSuccessful);

	/**
	 * Delegate fired when a session search query has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	 * Delegate fired when a session join request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * Delegate fired when a destroying an online session has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	virtual void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);	

	/* 
	 * Event triggered when a presence session is created
	 *
	 * @param SessionName name of session that was created
	 * @param bWasSuccessful was the create successful
	 */
	DECLARE_EVENT_TwoParams(APBGameSession, FOnCreatePresenceSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreatePresenceSessionComplete CreateSessionCompleteEvent;

	/* 
	 * Event triggered when a session is joined
	 *
	 * @param SessionName name of session that was joined
	 * @param bWasSuccessful was the create successful
	 */
	//DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnJoinSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	DECLARE_EVENT_OneParam(APBGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/*
	 * Event triggered after session search completes
	 */
	//DECLARE_EVENT(APBGameSession, FOnFindSessionsComplete);
	DECLARE_EVENT_OneParam(APBGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

public:

	/** Default number of players allowed in a game */
	static const int32 DEFAULT_NUM_PLAYERS = 8;

	/**
	 * Host a new online session
	 *
	 * @param UserId user that initiated the request
	 * @param SessionName name of session 
	 * @param bIsLAN is this going to hosted over LAN
	 * @param bIsPresence is the session to create a presence session
	 * @param MaxNumPlayers Maximum number of players to allow in the session
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	/**
	 * Find an online session
	 *
	 * @param UserId user that initiated the request
	 * @param SessionName name of session this search will generate
	 * @param bIsLAN are we searching LAN matches
	 * @param bIsPresence are we searching presence sessions
	 */
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);

	/**
	 * Joins one of the session in search results
	 *
	 * @param UserId user that initiated the request
	 * @param SessionName name of session 
	 * @param SessionIndexInSearchResults Index of the session in search results
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults);

	/**
	 * Joins a session via a search result
	 *
	 * @param SessionName name of session 
	 * @param SearchResult Session to join
	 *
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** @return true if any online async work is in progress, false otherwise */
	bool IsBusy() const;

	/**
	 * Get the search results found and the current search result being probed
	 *
	 * @param SearchResultIdx idx of current search result accessed
	 * @param NumSearchResults number of total search results found in FindGame()
	 *
	 * @return State of search result query
	 */
	EOnlineAsyncTaskState::Type GetSearchResultStatus(int32& SearchResultIdx, int32& NumSearchResults);

	/**
	 * Get the search results.
	 *
	 * @return Search results
	 */
	const TArray<FOnlineSessionSearchResult> & GetSearchResults() const;

	/** @return the delegate fired when creating a presence session */
	FOnCreatePresenceSessionComplete& OnCreateSessionComplete() { return CreateSessionCompleteEvent; }

	/** @return the delegate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }

	/** @return the delegate fired when search of session completes */
	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }

	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;
	void RegisterPlayer(APlayerController* NewPlayer, const TSharedPtr<const FUniqueNetId>& UniqueId, bool bWasFromInvite) override;
	void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;

	/** Handles to various registered delegates */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
};
