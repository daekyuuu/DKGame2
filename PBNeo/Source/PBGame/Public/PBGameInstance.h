// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBGame.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "PBNetServerInterface.h"
#include "PBGameInstance.generated.h"

DECLARE_DELEGATE(FMessageMenuButtonClicked);

class UPBGameInstance;
class FPBMessageMenu : public TSharedFromThis<FPBMessageMenu>
{

public:

	void Construct(TWeakObjectPtr<UPBGameInstance> InGameInstance, TWeakObjectPtr<ULocalPlayer> InPlayerOwner, const FText& Message, const FText& OKButtonText, const FText& CancelButtonText, const FName& InPendingNextState);

	void RemoveFromGameViewport();

	void HideDialogAndGotoNextState();

	void SetOKClickedDelegate(FMessageMenuButtonClicked	InButtonDelegate);

	void SetCancelClickedDelegate(FMessageMenuButtonClicked	InButtonDelegate);

private:

	/** Owning game instance */
	TWeakObjectPtr<UPBGameInstance> GameInstance;

	/** Local player that will have focus of the dialog box (can be NULL) */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	/** Cache the desired next state so we can advance to that after the confirmation dialog */
	FName PendingNextState;

	/** Handler for ok confirmation. */
	FReply OnClickedOK();
	FMessageMenuButtonClicked	OKButtonDelegate;

	FReply OnClickedCancel();
	FMessageMenuButtonClicked	CancelButtonDelegate;

};

class UPBLoginWidget;
class UPBWaitDialogWidget;
class UPBDialogConfirmWidget;
class UPBDialogNoticeWidget;
class UPBDialogReconnectWidget;
class UPBItemTableManager;
class UPBGameTableManager;
class UPBPacketManager;
class UPBNetClientInterface;
class UPBNetServerInterface;

class FVariantData;
class APBGameSession;

namespace PBGameInstanceState
{
	extern const FName None;
	extern const FName PendingInvite;
	extern const FName Login;
	extern const FName MainMenu;
	extern const FName MessageMenu;
	extern const FName LoadingScreen;
	extern const FName Playing;
}

/** This class holds the value of what message to display when we are in the "MessageMenu" state */
class FPBPendingMessage
{
public:
	FText	DisplayString;				// This is the display message in the main message body
	FText	OKButtonString;				// This is the ok button text
	FText	CancelButtonString;			// If this is not empty, it will be the cancel button text
	FName	NextState;					// Final destination state once message is discarded

	TWeakObjectPtr< ULocalPlayer > PlayerOwner;		// Owner of dialog who will have focus (can be NULL)
};

class FPBPendingInvite
{
public:
	FPBPendingInvite() : ControllerId(-1), UserId(nullptr), bPrivilegesCheckedAndAllowed(false) {}

	int32							 ControllerId;
	TSharedPtr< const FUniqueNetId > UserId;
	FOnlineSessionSearchResult 		 InviteResult;
	bool							 bPrivilegesCheckedAndAllowed;
};


USTRUCT(BlueprintType)
struct FPendingStreamLevel
{
	GENERATED_USTRUCT_BODY()

	FString LevelName;
	FString CallbackFuncOnLoaded;
	bool bLevelVisibleOnLoaded;

	void Cleanup()
	{
		LevelName.Empty();
		CallbackFuncOnLoaded.Empty();
		bLevelVisibleOnLoaded = true;
	}
};

USTRUCT(BlueprintType)
struct FMapData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Map)
	TAssetPtr<UWorld> MapAsset;

	UPROPERTY(EditDefaultsOnly, Category = Map)
	FString MapDisplayName;
};

UCLASS(config=Game)
class UPBGameInstance : public UGameInstance
{
	friend class APBPlayerController;

public:
	GENERATED_UCLASS_BODY()

public:
	bool Tick(float DeltaSeconds);

	APBGameSession* GetGameSession() const;

	void Init() override;
	void Shutdown() override;
	void StartGameInstance() override;
#if WITH_EDITOR
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif

	void InitGameTables();
	UPBItemTableManager* GetItemTableManager() { return ItemTableManager; }
	UPBGameTableManager* GetGameTableManager() { return GameTableManager; }
	UPBPacketManager* GetPacketManager() { return PacketManager;  }
	UPBNetClientInterface* GetNetClientInterface() { return NetClientInterface;  }
	UPBNetServerInterface* GetNetServerInterface() { return NetServerInterface;  }
	UPBRoomUserInfoManager* GetRoomUserInfoManager() { return RoomUserManager; }

	// Dedicated Server Control
	void StartDediGameInstance();
	bool IsSpawnedByDediManager();
	void DediConnectToManager();
	FDelegateHandle DediHandleConnected_Handle;
	void DediHandleConnected(EPBNetServerState PrevState, EPBNetServerState NewState);
	FDelegateHandle DediHandleLoggedIn_Handle;
	void DediHandleLoggedIn(EPBNetServerState PrevState, EPBNetServerState NewState);
	FDelegateHandle DediHandleLoadMapSignal_Handle;
	void DediHandleLoadMapSignal(int32 MapTableIndex);
	FDelegateHandle DediHandleGameReady_Handle;
	void DediHandleGameReady();
	FDelegateHandle DediHandleShutdownSignal_Handle;
	void DediHandleShutdownSignal();
	FDelegateHandle DediHandleKillSignal_Handle;
	void DediHandleKillSignal();

	// Notifies from GameMode
	DECLARE_EVENT(UPBGameInstance, FOnGameReadyEvt)
	FOnGameReadyEvt OnGameReadyEvt;

	bool ReturnToMainMenu(bool UnexpectedFromClient);

private:
	int32 CurrentRoomIdx;

public:
	virtual bool DelayPendingNetGameTravel() override;
	
	ULevelStreaming* GetStreamingLevel(FString MapName);
	ULevelStreaming* LoadStreamingLevel(FPendingStreamLevel& InPendingStream);
	void UnloadStreamingLevel(FString MapName, FString CallbackFunction = TEXT(""));
	void ShowStreamingLevel(FString MapName);
	void SetCurrentStreamingLevelName(FString MapName);

	FString PreviousStreamingLevelName;
	FString CurrentStreamingLevelName;
	FPendingStreamLevel PendingStream;
		
	UPROPERTY(config)
	float MininumLoadingScreenDuration;

	float LoadingScreenStartTime;
	bool bLoadingScreenFinished;

	bool bDelayPendingNetGameTravel;

	FString GetMapNameFromTable(uint32 MapIdx);
	FString GetFirstGameStreamingLevel();

public:
	void UseOnceSimpleLoadMap();

	bool LoadLoginMap();
	bool LoadLobbyMap();
	bool LoadFrontEndMap(const FString& MapName);

	bool EnterGame(FString DediURL, uint32 MapIdx);
	bool EnterGameInternal(FString TravelURL);

	// Mainly for testing
	void PendingEnterGame();
	FString PendingGameURL;
	FString PendingGameMap;

	void HandleEnterGameCountdownStarted();

	void StartLoadingScreen();
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnLoadingScreenFinished();

public:
	void OnPreLoadMap(const FString& MapName);
	void OnPostLoadMap();

protected:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPostLoadLoginMap();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPostLoadLobbyMap();
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPostUnloadLobbyMap();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void Client_OnPostLoadPlayMap();
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void Server_OnPostLoadPlayMap();

public:
	/** Sends the game to the specified state. */
	void GotoState(FName NewState);

	/** Obtains the initial welcome state, which can be different based on platform */
	FName GetInitialState();

	/** Sends the game to the initial startup/frontend state  */
	void GotoInitialState();

	FName GetCurrentState();
	bool IsCurrentState(FName StateName);
	bool IsPedingState(FName StateName);
	/**
	* Creates the message menu, clears other menus and sets the KingState to Message.
	*
	* @param	Message				Main message body
	* @param	OKButtonString		String to use for 'OK' button
	* @param	CancelButtonString	String to use for 'Cancel' button
	* @param	NewState			Final state to go to when message is discarded
	*/
	void ShowMessageThenGotoState( const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting = true, TWeakObjectPtr< ULocalPlayer > PlayerOwner = nullptr );

	TSharedPtr< const FUniqueNetId > GetUniqueNetIdFromControllerId( const int ControllerId );

	/** Returns true if the game is in online mode */
	bool GetIsOnline() const { return bIsOnline; }

	/** Sets the online mode of the game */
	void SetIsOnline(bool bInIsOnline);

	/** Sets the controller to ignore for pairing changes. Useful when we are showing external UI for manual profile switching. */
	void SetIgnorePairingChangeForControllerId( const int32 ControllerId );

	/** Returns true if the passed in local player is signed in and online */
	bool IsLocalPlayerOnline(ULocalPlayer* LocalPlayer);

	/** Returns true if owning player is online. Displays proper messaging if the user can't play */
	bool ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer);

	/** Flag the local player when they quit the game */
	void LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const;

	// Generic confirmation handling (just hide the dialog)
	FReply OnConfirmGeneric();
	bool HasLicense() const { return bIsLicensed; }

	/** Start task to get user privileges. */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId);

	/** Common cleanup code for any Privilege task delegate */
	void CleanupOnlinePrivilegeTask();

	/** Show approved dialogs for various privileges failures */
	void DisplayOnlinePrivilegeFailureDialogs(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

public:
	void PlayBGM();
	void StopBGM();

	/** level music */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BGM)
	USoundCue* LobbyBGM;

protected:
	UPROPERTY()
	UAudioComponent* LobbyBGMComp;

	UPROPERTY()
	UPBItemTableManager* ItemTableManager;
	UPROPERTY()
	UPBGameTableManager* GameTableManager;
	UPROPERTY()
	UPBPacketManager* PacketManager;
	UPROPERTY()
	UPBNetClientInterface* NetClientInterface;
	UPROPERTY()
	UPBNetServerInterface* NetServerInterface;
	UPROPERTY()
	UPBRoomUserInfoManager* RoomUserManager;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LoadingScreen)
	TAssetPtr<UTexture2D> LoadingScreenTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MainMenu)
	TSubclassOf<UPBLoginWidget> LoginClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SystemMessage)
	TSubclassOf<class UPBWidget_SystemMessageListBox> SystemMessageWidgetClass;
	class UPBWidget_SystemMessageListBox *SystemMessageListBox;
	//TMap<int64 /*TimeStamp as Unique ID*/, FString /* Message */> SystemMessageContexts;
	void CreateWidgetSystemMessageListBox();
	void DestroyWidgetSystemMessageListBox();
	class UPBWidget_SystemMessageListBox *GetWidgetSystemMessageListBox() const;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Screen)
	TSubclassOf<class UPBWidget_ScreenFadeInOut> ScreenFadeInOutClass;
	class UPBWidget_ScreenFadeInOut *ScreenFadeInOut;
	void CreateWidgetScreenFadeInOut();
	void DestroyWidgetScreenFadeInOut();
	class UPBWidget_ScreenFadeInOut *GetWidgetScreenFadeInOut() const;
	void StartFadeOutScreen(float DelayInSeconds = 0.0f);
	void StartFadeInScreen(float DelayInSeconds = 0.0f);

	/* ------------------------------------------------------------------------------- */
	// Lobby
	/* ------------------------------------------------------------------------------- */
public:
	UPROPERTY(EditAnywhere, Category = "Lobby")
	TSubclassOf<class UPBLobby> LobbyClass;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	class UPBLobby* LobbyInstance;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	class UPBLobby* GetLobbyInstance();

private:
	class UPBLobby* BuildLobbyInstance();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MainMenu)
	TSubclassOf<UPBWaitDialogWidget> WaitDialogClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MainMenu)
	TArray<FMapData> AvailableMaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dialog)
	TSubclassOf<UPBDialogConfirmWidget> ConfirmDialogClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dialog)
	TSubclassOf<UPBDialogNoticeWidget> NoticeDialogClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dialog)
	TSubclassOf<UPBDialogReconnectWidget> ReconnectDialogClass;

private:
	// 클라이언트 / 서버 공통
	UPROPERTY(config)
	FString StreamingRootMap;

	UPROPERTY(config)
	FString LoginMap;

	UPROPERTY(config)
	FString MainMenuMap;	

	UPROPERTY(config)
	FString TestGameMap;

	FName PreviousState;
	FName CurrentState;
	FName PendingState;

	FPBPendingMessage PendingMessage;

	FPBPendingInvite PendingInvite;

	/** Whether the match is online or not */
	bool bIsOnline;

	/** Whether the user has an active license to play the game */
	bool bIsLicensed;

	UPROPERTY()
	UPBLoginWidget* LoginUI;

	UPROPERTY()
	UPBWaitDialogWidget* WaitDialogUI;

	/** Message menu (Shown in the even of errors - unable to connect etc) */
	TSharedPtr<FPBMessageMenu> MessageMenuUI;

	/** Controller to ignore for pairing changes. -1 to skip ignore. */
	int32 IgnorePairingChangeForControllerId;

	/** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
	EOnlineServerConnectionStatus::Type	CurrentConnectionStatus;

	/** Delegate for callbacks to Tick */
	FTickerDelegate TickDelegate;

	/** Handle to various registered delegates */
	FDelegateHandle TickDelegateHandle;
	FDelegateHandle TravelLocalSessionFailureDelegateHandle;
	
	void HandleNetworkConnectionStatusChanged(  EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus );

	void HandleInviteAccepted(const bool bWasSuccess, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	void HandleSessionFailure( const FUniqueNetId& NetId, ESessionFailure::Type FailureType );
	
	/** Delegate function executed after checking privileges for starting quick match */
	void OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

	void MaybeChangeState();
	void EndCurrentState();
	void BeginNewState(FName NewState);

	void BeginPendingInviteState();
	void BeginLoginState();
	void BeginMainMenuState();
	void BeginMessageMenuState();
	void BeginLoadingScreenState();
	void BeginPlayingState();

	void EndPendingInviteState();
	void EndLoginState();
	void EndMainMenuState();
	void EndMessageMenuState();
	void EndLoadingScreenState();
	void EndPlayingState();

	// 지금 쓰지 않는다. 일단 남겨둠
	void ShowLoadingScreen();
	void AddNetworkFailureHandlers();
	void RemoveNetworkFailureHandlers();

	/** Called when there is an error trying to travel to a local session */
	void TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ErrorString);

	/**
	* Creates the message menu, clears other menus and sets the KingState to Message.
	*
	* @param	Message				Main message body
	* @param	OKButtonString		String to use for 'OK' button
	* @param	CancelButtonString	String to use for 'Cancel' button
	*/
	void ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString);

	/** Sets a rich presence string for all local players. */
	void SetPresenceForLocalPlayers(const FVariantData& PresenceData);

	/** Show messaging and punt to welcome screen */
	void HandleSignInChangeMessaging();

	// OSS delegates to handle
	void HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId);

	// Callback to pause the game when the OS has constrained our app.
	void HandleAppWillDeactivate();

	// Callback occurs when game being suspended
	void HandleAppSuspend();

	// Callback occurs when game resuming
	void HandleAppResume();

	void HandleAppTerminate();

	// Callback to process game licensing change notifications.
	void HandleAppLicenseUpdate();

	// Callback to handle safe frame size changes.
	void HandleSafeFrameChanged();

	// Callback to handle controller connection changes.
	void HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex);

	// Callback to handle controller pairing changes.
	FReply OnPairingUsePreviousProfile();

	// Callback to handle controller pairing changes.
	FReply OnPairingUseNewProfile();

	// Callback to handle controller pairing changes.
	void HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser);

	void HandleLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	// Handle confirming the controller disconnected dialog.
	FReply OnControllerReconnectConfirm();	

protected:
	bool HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld) override;
};


