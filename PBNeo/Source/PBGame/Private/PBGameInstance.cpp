// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	PBGameInstance.cpp
=============================================================================*/

#include "PBGame.h"
#include "PBGameInstance.h"
#include "OnlineKeyValuePair.h"
#include "PBGameViewportClient.h"
#include "Player/PBPlayerController_Menu.h"
#include "Player/PBPlayerState.h"
#include "Online/PBGameSession.h"
#include "PBWaitDialogWidget.h"
#include "Table/Item/PBItemTableManager.h"
#include "Table/Game/PBGameTableManager.h"
#include "WidgetComponent.h"
#include "PBLobby.h"
#include "PBWidgetPage.h"
#include "PBClientPacketManager.h"
#include "PBNetClientInterface.h"
#include "PBNetServerInterface.h"
#include "Online/PBRoomUserInfoManager.h"
#include "PBMapTable.h"
#include "PBLoginWidget.h"
#include "Weapons/PBWeapon.h"
#include "UMG/Common/PBWidget_SystemMessageListBox.h"
#include "UMG/Common/PBWidget_ScreenFadeInOut.h"

void FPBMessageMenu::Construct(TWeakObjectPtr<UPBGameInstance> InGameInstance, TWeakObjectPtr<ULocalPlayer> InPlayerOwner, const FText& Message, const FText& OKButtonText, const FText& CancelButtonText, const FName& InPendingNextState)
{
	GameInstance = InGameInstance;
	PlayerOwner = InPlayerOwner;
	PendingNextState = InPendingNextState;

	if (ensure(GameInstance.IsValid()))
	{
		UPBGameViewportClient* PBViewport = Cast<UPBGameViewportClient>(GameInstance->GetGameViewportClient());

		if (PBViewport)
		{
			// Hide the previous dialog
			PBViewport->HideDialog();

			// Show the new one
			PBViewport->ShowDialog(
				PlayerOwner,
				EPBDialogType::Generic,
				Message,
				OKButtonText,
				CancelButtonText,
				FOnClicked::CreateRaw(this, &FPBMessageMenu::OnClickedOK),
				FOnClicked::CreateRaw(this, &FPBMessageMenu::OnClickedCancel)
			);
		}
	}
}

void FPBMessageMenu::RemoveFromGameViewport()
{
	if (ensure(GameInstance.IsValid()))
	{
		UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GameInstance->GetGameViewportClient());

		if (PBViewport)
		{
			// Hide the previous dialog
			PBViewport->HideDialog();
		}
	}
}

void FPBMessageMenu::HideDialogAndGotoNextState()
{
	RemoveFromGameViewport();

	if (ensure(GameInstance.IsValid()))
	{
		GameInstance->GotoState(PendingNextState);
	}
};

FReply FPBMessageMenu::OnClickedOK()
{
	OKButtonDelegate.ExecuteIfBound();
	HideDialogAndGotoNextState();
	return FReply::Handled();
}

FReply FPBMessageMenu::OnClickedCancel()
{
	CancelButtonDelegate.ExecuteIfBound();
	HideDialogAndGotoNextState();
	return FReply::Handled();
}

void FPBMessageMenu::SetOKClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
	OKButtonDelegate = InButtonDelegate;
}

void FPBMessageMenu::SetCancelClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
	CancelButtonDelegate = InButtonDelegate;
}

namespace PBGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName Login = FName(TEXT("Login"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName MessageMenu = FName(TEXT("MessageMenu"));
	const FName LoadingScreen = FName(TEXT("LoadingScreen"));
	const FName Playing = FName(TEXT("Playing"));
}

UPBGameInstance::UPBGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsOnline(true) // Default to online
	, bIsLicensed(true) // Default to licensed (should have been checked by OS on boot)
{
	PreviousState = PBGameInstanceState::None;
	CurrentState = PBGameInstanceState::None;
	ItemTableManager = nullptr;
	GameTableManager = nullptr;
	MininumLoadingScreenDuration = 7.f;
	LoadingScreenStartTime = 0.f;
	PendingStream.LevelName.Empty();
	bLoadingScreenFinished = true;
	bDelayPendingNetGameTravel = false;
}

void UPBGameInstance::Init()
{
	Super::Init();

	IgnorePairingChangeForControllerId = -1;
	CurrentConnectionStatus = EOnlineServerConnectionStatus::Connected;

	// game requires the ability to ID users.
	const auto OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	const auto IdentityInterface = OnlineSub->GetIdentityInterface();
	check(IdentityInterface.IsValid());

	const auto SessionInterface = OnlineSub->GetSessionInterface();
	check(SessionInterface.IsValid());

	// bind any OSS delegates we needs to handle
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		IdentityInterface->AddOnLoginStatusChangedDelegate_Handle(i, FOnLoginStatusChangedDelegate::CreateUObject(this, &UPBGameInstance::HandleUserLoginChanged));
	}

	IdentityInterface->AddOnControllerPairingChangedDelegate_Handle(FOnControllerPairingChangedDelegate::CreateUObject(this, &UPBGameInstance::HandleControllerPairingChanged));
	IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UPBGameInstance::HandleLoginComplete));

	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UPBGameInstance::HandleAppWillDeactivate);

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UPBGameInstance::HandleAppSuspend);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UPBGameInstance::HandleAppResume);
	FCoreDelegates::ApplicationWillTerminateDelegate.AddUObject(this, &UPBGameInstance::HandleAppTerminate);
	FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &UPBGameInstance::HandleSafeFrameChanged);
	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UPBGameInstance::HandleControllerConnectionChange);
	FCoreDelegates::ApplicationLicenseChange.AddUObject(this, &UPBGameInstance::HandleAppLicenseUpdate);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPBGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UPBGameInstance::OnPostLoadMap);

	OnlineSub->AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate::CreateUObject(this, &UPBGameInstance::HandleNetworkConnectionStatusChanged));

	SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UPBGameInstance::HandleInviteAccepted));
	SessionInterface->AddOnSessionFailureDelegate_Handle(FOnSessionFailureDelegate::CreateUObject(this, &UPBGameInstance::HandleSessionFailure));

	// Register delegate for ticker callback
	TickDelegate = FTickerDelegate::CreateUObject(this, &UPBGameInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	InitGameTables();

	if (!IsRunningDedicatedServer())
	{
		PacketManager = NewObject<UPBClientPacketManager>(this, UPBClientPacketManager::StaticClass());
	}
	else
	{
		PacketManager = NewObject<UPBServerPacketManager>(this, UPBServerPacketManager::StaticClass());
	}

	if (PacketManager)
	{
		PacketManager->Init();
	}

	if (!IsRunningDedicatedServer())
	{
		RoomUserManager = NewObject<UPBRoomUserInfoManager>(this, UPBRoomUserInfoManager::StaticClass());

		NetClientInterface = NewObject<UPBNetClientInterface>(this, UPBNetClientInterface::StaticClass());
		if (NetClientInterface)
		{
			NetClientInterface->Init();
			NetClientInterface->OnBattleStartCountdownEvt.AddUObject(this, &UPBGameInstance::HandleEnterGameCountdownStarted);
		}		
	}
	else
	{
		NetServerInterface = NewObject<UPBNetServerInterface>(this, UPBNetServerInterface::StaticClass());
		if (NetServerInterface)
		{
			NetServerInterface->Init();
		}
	}
}

void UPBGameInstance::Shutdown()
{
	if (PacketManager)
	{
		PacketManager->ShutDown();
	}
	if (NetClientInterface)
	{
		NetClientInterface->ShutDown();
	}
	if (NetServerInterface)
	{
		NetServerInterface->ShutDown();
	}

	Super::Shutdown();

	// Unregister ticker delegate
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UPBGameInstance::HandleNetworkConnectionStatusChanged(EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus)
{
	UE_LOG(LogOnlineGame, Warning, TEXT("UPBGameInstance::HandleNetworkConnectionStatusChanged: %s"), EOnlineServerConnectionStatus::ToString(ConnectionStatus));

#if PB_CONSOLE_UI
	// If we are disconnected from server, and not currently at (or heading to) the welcome screen
	// then display a message on consoles
	if (bIsOnline &&
		PendingState != PBGameInstanceState::Login &&
		CurrentState != PBGameInstanceState::Login &&
		ConnectionStatus != EOnlineServerConnectionStatus::Connected)
	{
		UE_LOG(LogOnlineGame, Log, TEXT("UPBGameInstance::HandleNetworkConnectionStatusChanged: Going to main menu"));

		// Display message on consoles
#if PLATFORM_XBOXONE
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost.");
#elif PLATFORM_PS4
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to \"PSN\" has been lost.");
#else
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection has been lost.");
#endif
		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

		ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu);
	}

	CurrentConnectionStatus = ConnectionStatus;
#endif
}

void UPBGameInstance::HandleInviteAccepted(const bool bWasSuccess, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	UE_LOG(LogOnline, Verbose, TEXT("HandleSessionUserInviteAccepted: bSuccess: %d, ControllerId: %d, User: %s"), bWasSuccess, ControllerId, UserId.IsValid() ? *UserId->ToString() : TEXT("NULL"));

	if (!bWasSuccess)
	{
		return;
	}

	if (!InviteResult.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no search result."));
		return;
	}

	if (!UserId.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no user."));
		return;
	}

	// Set the pending invite, and then go to the initial screen, which is where we will process it
	PendingInvite.ControllerId = ControllerId;
	PendingInvite.UserId = UserId;
	PendingInvite.InviteResult = InviteResult;
	PendingInvite.bPrivilegesCheckedAndAllowed = false;

	GotoState(PBGameInstanceState::PendingInvite);
}

void UPBGameInstance::HandleSessionFailure(const FUniqueNetId& NetId, ESessionFailure::Type FailureType)
{
	UE_LOG(LogOnlineGame, Warning, TEXT("UPBGameInstance::HandleSessionFailure: %u"), (uint32)FailureType);

#if PB_CONSOLE_UI
	// If we are not currently at (or heading to) the welcome screen then display a message on consoles
	if (bIsOnline &&
		PendingState != PBGameInstanceState::Login &&
		CurrentState != PBGameInstanceState::Login)
	{
		UE_LOG(LogOnlineGame, Log, TEXT("UPBGameInstance::HandleSessionFailure: Going to main menu"));

		// Display message on consoles
#if PLATFORM_XBOXONE
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost.");
#elif PLATFORM_PS4
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection to PSN has been lost.");
#else
		const FText ReturnReason = NSLOCTEXT("NetworkFailures", "ServiceUnavailable", "Connection has been lost.");
#endif
		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

		ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu);
	}
#endif
}

void UPBGameInstance::OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	CleanupOnlinePrivilegeTask();

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		if (UserId == *PendingInvite.UserId)
		{
			PendingInvite.bPrivilegesCheckedAndAllowed = true;
		}
	}
	else
	{
		DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
		GotoState(PBGameInstanceState::Login);
	}
}

#if WITH_EDITOR
FGameInstancePIEResult UPBGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	FGameInstancePIEResult bResult = Super::StartPlayInEditorGameInstance(LocalPlayer, Params);

	if (bResult.IsSuccess())
	{
		CreateWidgetSystemMessageListBox();
		CreateWidgetScreenFadeInOut();
	}


	GotoState(PBGameInstanceState::Playing);

	return bResult;
}
#endif

//독립형으로 실행시
void UPBGameInstance::StartGameInstance()
{
	CreateWidgetSystemMessageListBox();
	CreateWidgetScreenFadeInOut();

	CurrentRoomIdx = 0;

// If a map/network-address is specified when creating client-program, go-to-map/connect-to-net-address under playing-state
#if PLATFORM_WINDOWS
	if (!IsRunningDedicatedServer())
	{
		// Note : 에디터에서 독립형으로 실행시 서버,클라 모두 이쪽으로 들어온다.

		TCHAR Parm[4096] = TEXT("");
		const TCHAR* Cmd = FCommandLine::Get();
		FString CmdString(Cmd);

		FURL DefaultURL;
		DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

		// Cmd 에 [맵이름, IP, ?] 등이 하나라도 있어야 URL.Valid == true 가 됩니다.
		FURL URL(&DefaultURL, Cmd, TRAVEL_Partial);

		const bool bClientStandalonePlayInEditor = CmdString.Contains(TEXT("-PIEVIACONSOLE"));
		if (bClientStandalonePlayInEditor)
		{
			if (FParse::Token(Cmd, Parm, ARRAY_COUNT(Parm), 0) && Parm[0] != '-')
			{
				FURL NewURL(&DefaultURL, Parm, TRAVEL_Partial);
				URL = NewURL;
			}
		}		
	
		if (URL.Valid)
		{
			// Load the Streaming root map first.
			const bool bNeedsStreamingLevelLoad = CmdString.Contains(TEXT("-streamlevel"));

			if (bNeedsStreamingLevelLoad)
			{
				// 신속한 테스트를 위해 먼저 로비맵을 로딩해놓고 그 다음 게임맵으로 진입하게 설정함
				if (LoadLobbyMap())
				{
					PendingGameMap = TestGameMap.IsEmpty() ? GetFirstGameStreamingLevel() : TestGameMap;
					PendingGameURL = CmdString;	// Include IP
				}
			}				

			//////////////////////////////////////////////////////////////////////////
			
			UEngine* const Engine = GetEngine();
			
			FString Error;
			const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);

			if (BrowseRet == EBrowseReturnVal::Success)
			{
				// Success, we loaded the map, go directly to playing state
				GotoState(bNeedsStreamingLevelLoad ? PBGameInstanceState::LoadingScreen : PBGameInstanceState::Playing);
				return;
			}
			else if (BrowseRet == EBrowseReturnVal::Pending)
			{
				// Pending, we are connecting to a server in client mode
				AddNetworkFailureHandlers();
				GotoState(bNeedsStreamingLevelLoad ? PBGameInstanceState::LoadingScreen : PBGameInstanceState::Playing);
				return;
			}
			else
			{
				UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s: %s. Please check the log for errors."), *URL.Map, *URL.Host, *Error));
			}
		}
	}
#endif

#if PLATFORM_WINDOWS
	if (IsRunningDedicatedServer())
	{
		StartDediGameInstance();

		if (IsSpawnedByDediManager())
		{
			UE_LOG(LogOnline, Log, TEXT("Connecting to GameServerManager"));
			DediConnectToManager();
			return;
		}
		else
		{
			UE_LOG(LogOnline, Log, TEXT("Standalone Dedi Server not managed by GameServerManager"));
			return;
		}
	}
#endif

	GotoInitialState();
}

//Dedi or listen 서버
void UPBGameInstance::StartDediGameInstance()
{	
	const TCHAR* Cmd = FCommandLine::Get();		
	FString CmdString(Cmd);

	FURL DefaultURL;
	DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

	// Cmd 에 [맵이름, IP, ?] 등이 하나라도 있어야 URL.Valid == true 가 됩니다.
	FURL URL(&DefaultURL, Cmd, TRAVEL_Partial);

	if (URL.Valid)
	{
		// Load the Streaming root map first.
		FString StreamingRootLevelName = DefaultURL.Map.IsEmpty() ? StreamingRootMap : DefaultURL.Map;

		const bool bNeedsStreamingLevelLoad = StreamingRootLevelName.Contains(TEXT("Root"));

		if (bNeedsStreamingLevelLoad && URL.Map != DefaultURL.Map)
		{
			FString PendingMapName = URL.Map;	//Streaming level child (Load PlayMap after root level is loading complete)

			URL.Map = StreamingRootLevelName;	//Streaming level Root (make sure Load root level first)

			PendingStream.LevelName = PendingMapName;
			PendingStream.CallbackFuncOnLoaded = TEXT("Server_OnPostLoadPlayMap");
			PendingStream.bLevelVisibleOnLoaded = true;
		}		

		//////////////////////////////////////////////////////////////////////////
		UEngine* const Engine = GetEngine();

		FString Error;
		const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);

		if (BrowseRet == EBrowseReturnVal::Success)
		{
			// RoomIdx 설정	
			CurrentRoomIdx = FCString::Atoi(URL.GetOption(TEXT("RoomIdx="), TEXT("")));

			// Success, we loaded the map, go directly to playing state
			GotoState(PBGameInstanceState::Playing);
			return;
		}
		else
		{
			UE_LOG(LogOnline, Log, TEXT("Dedicated server map loading failure!"));
			check(false); // This should never happen...
			return;
		}
	}
}

bool UPBGameInstance::IsSpawnedByDediManager()
{
	const TCHAR* Cmd = FCommandLine::Get();
	return FCString::Strifind(Cmd, TEXT("SpawnedByGameServerManager")) != NULL;
}

void UPBGameInstance::DediConnectToManager()
{
	UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
	ensure(ServerInterface);
	
	ServerInterface->ConnectToServer("127.0.0.1", 36100);
	DediHandleConnected_Handle = ServerInterface->OnStateChangedEvt.AddUObject(this, &UPBGameInstance::DediHandleConnected);
}

void UPBGameInstance::DediHandleConnected(EPBNetServerState PrevState, EPBNetServerState NewState)
{
	if (PrevState == EPBNetServerState::NotConnected && NewState == EPBNetServerState::Connected)
	{
		UE_LOG(LogOnline, Log, TEXT("Connected to GameServerManager"));
		UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
		ensure(ServerInterface);

		ServerInterface->OnStateChangedEvt.Remove(DediHandleConnected_Handle);

		ensure(GetWorld());
		ServerInterface->Login((uint16)GetWorld()->URL.Port, CurrentRoomIdx);
		DediHandleLoggedIn_Handle = ServerInterface->OnStateChangedEvt.AddUObject(this, &UPBGameInstance::DediHandleLoggedIn);
	}
}

void UPBGameInstance::DediHandleLoggedIn(EPBNetServerState PrevState, EPBNetServerState NewState)
{
	if (PrevState == EPBNetServerState::Connected && NewState == EPBNetServerState::LoggedIn)
	{
		UE_LOG(LogOnline, Log, TEXT("Logged in at GameServerManager with listen port: %d"), GetWorld()->URL.Port);
		UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
		ensure(ServerInterface);

		ServerInterface->OnStateChangedEvt.Remove(DediHandleLoggedIn_Handle);

		DediHandleLoadMapSignal_Handle = ServerInterface->OnLoadMapSignalEvt.AddUObject(this, &UPBGameInstance::DediHandleLoadMapSignal);
		DediHandleShutdownSignal_Handle = ServerInterface->OnShutdownSignalEvt.AddUObject(this, &UPBGameInstance::DediHandleShutdownSignal);
	}
}

void UPBGameInstance::DediHandleLoadMapSignal(int32 MapTableIndex)
{
	UE_LOG(LogOnline, Log, TEXT("Received loadmap signal, loading map with table index %d"), MapTableIndex);
	UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
	ensure(ServerInterface);

	ensure(GetWorld());
	UPBMapTable* MapTable = Cast<UPBMapTable>(UPBGameplayStatics::GetGameTableManager(this)->GetTable(EPBTableType::GameMap));
	if (MapTable)
	{
		FPBMapTableData* Data = MapTable->GetData(MapTableIndex);
		if (Data == nullptr)
		{
			UE_LOG(LogOnline, Warning, TEXT("Can't find map info from table index %d, you should check map table in 'Content/Tables/tbMaps.uasset'"), MapTableIndex);
		}

		if (Data)
		{
			FString MapName = Data->BPClass.GetLongPackageName();
			FString ModeString = Data->ModeShortName;
			int32 CurrentPort = GetWorld()->URL.Port; // Make sure we are using same port as we load new map!
			const FString TravelURL = FString::Printf(TEXT("%s?game=%s-PORT=%d"), *MapName, *ModeString, CurrentPort);

			FURL DefaultURL;
			FURL URL(&DefaultURL, *TravelURL, TRAVEL_Partial);

			if (URL.Valid)
			{
				// Load the Streaming root map first.
				FString StreamingRootLevelName = DefaultURL.Map.IsEmpty() ? StreamingRootMap : DefaultURL.Map;

				const bool bNeedsStreamingLevelLoad = StreamingRootLevelName.Contains(TEXT("Root"));

				if (bNeedsStreamingLevelLoad && URL.Map != DefaultURL.Map)
				{
					FString PendingMapName = URL.Map;	//this is Streaming level child (Load PendingMap after root loading is complete)

					URL.Map = StreamingRootLevelName;	//this is Streaming level Root (Load root map first)

					PendingStream.LevelName = PendingMapName;
					PendingStream.CallbackFuncOnLoaded = TEXT("Server_OnPostLoadPlayMap");
					PendingStream.bLevelVisibleOnLoaded = true;
				}

				//////////////////////////////////////////////////////////////////////////
				UEngine* const Engine = GetEngine();

				FString Error;
				const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);

				if (BrowseRet == EBrowseReturnVal::Success)
				{
					// Server_OnPostLoadPlayMap() 함수에서 최종 처리되기 때문에 아래 로직은 주석처리함
					//DediHandleGameReady_Handle = OnGameReadyEvt.AddUObject(this, &UPBGameInstance::DediHandleGameReady);
					return;
				}
				else
				{
					UE_LOG(LogOnline, Log, TEXT("Dedicated server map loading failure!"));
					check(false); // This should never happen...
					return;
				}
			}

			//if (GetWorld()->ServerTravel(TravelURL))
			//{
			//	DediHandleGameReady_Handle = OnGameReadyEvt.AddUObject(this, &UPBGameInstance::DediHandleGameReady);
			//	return;
			//}
		}
	}

	ServerInterface->AckLoadMapSignal(false);
}

void UPBGameInstance::DediHandleGameReady()
{
	UE_LOG(LogOnline, Log, TEXT("Game is ready to receive new players, acknowleding map loaded to game manager"));
	UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
	ensure(ServerInterface);

	OnGameReadyEvt.Remove(DediHandleGameReady_Handle);

	ServerInterface->AckLoadMapSignal(true);
}

void UPBGameInstance::DediHandleShutdownSignal()
{
	UE_LOG(LogOnline, Log, TEXT("Received shutdown signal, acknowledging shutdown"));
	UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
	ensure(ServerInterface);

	ServerInterface->OnLoadMapSignalEvt.Remove(DediHandleLoadMapSignal_Handle);
	ServerInterface->OnShutdownSignalEvt.Remove(DediHandleShutdownSignal_Handle);

	ServerInterface->AckShutdownSignal();
	DediHandleKillSignal_Handle = ServerInterface->OnKillSignalEvt.AddUObject(this, &UPBGameInstance::DediHandleKillSignal);
}

void UPBGameInstance::DediHandleKillSignal()
{
	UE_LOG(LogOnline, Log, TEXT("Received kill signal, terminating"));
	UPBNetServerInterface* ServerInterface = UPBNetServerInterface::Get(this);
	ensure(ServerInterface);

	ServerInterface->OnKillSignalEvt.Remove(DediHandleKillSignal_Handle);

	FGenericPlatformMisc::RequestExit(false);
}

void UPBGameInstance::InitGameTables()
{
	if (ItemTableManager == nullptr)
	{
		ItemTableManager = NewObject<UPBItemTableManager>(this);
	}

	if (GameTableManager == nullptr)
	{
		GameTableManager = NewObject<UPBGameTableManager>(this);
	}		
}

FName UPBGameInstance::GetInitialState()
{
	return PBGameInstanceState::Login;
}

void UPBGameInstance::GotoInitialState()
{
	GotoState(GetInitialState());
}

FName UPBGameInstance::GetCurrentState()
{
	return CurrentState;
}

bool UPBGameInstance::IsCurrentState(FName StateName)
{
	return (CurrentState == StateName);
}

bool UPBGameInstance::IsPedingState(FName StateName)
{
	return (PendingState == StateName);
}

void UPBGameInstance::ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting, TWeakObjectPtr< ULocalPlayer > PlayerOwner)
{
	UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Message: %s, NewState: %s"), *Message.ToString(), *NewState.ToString());

	const bool bAtLogin = PendingState == PBGameInstanceState::Login || CurrentState == PBGameInstanceState::Login;

	// Never override the login
	if (bAtLogin)
	{
		UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (at login)."));
		return;
	}

	const bool bAlreadyAtMessageMenu = PendingState == PBGameInstanceState::MessageMenu || CurrentState == PBGameInstanceState::MessageMenu;
	const bool bAlreadyAtDestState = PendingState == NewState || CurrentState == NewState;

	// If we are already going to the message menu, don't override unless asked to
	if (bAlreadyAtMessageMenu && PendingMessage.NextState == NewState && !OverrideExisting)
	{
		UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 1)."));
		return;
	}

	// If we are already going to the message menu, and the next dest is login, don't override
	if (bAlreadyAtMessageMenu && PendingMessage.NextState == PBGameInstanceState::Login)
	{
		UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 2)."));
		return;
	}

	// If we are already at the dest state, don't override unless asked
	if (bAlreadyAtDestState && !OverrideExisting)
	{
		UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 3)"));
		return;
	}

	PendingMessage.DisplayString = Message;
	PendingMessage.OKButtonString = OKButtonString;
	PendingMessage.CancelButtonString = CancelButtonString;
	PendingMessage.NextState = NewState;
	PendingMessage.PlayerOwner = PlayerOwner;

	if (CurrentState == PBGameInstanceState::MessageMenu)
	{
		UE_LOG(LogOnline, Log, TEXT("ShowMessageThenGotoState: Forcing new message"));
		EndMessageMenuState();
		BeginMessageMenuState();
	}
	else
	{
		GotoState(PBGameInstanceState::MessageMenu);
	}
}

void UPBGameInstance::ShowLoadingScreen()
{
	////	For LoadMap, we use the MoviePlayer interface to show the load screen
	////  This is necessary since LoadMap is a blocking call, and our viewport loading screen won't get updated
	////  We can't use MoviePlayer for seamless travel though
	////  In this case, we just add a widget to the viewport, and have it update on the main thread
	////  To simplify things, we just do both, one will cover the other if they both show at the same time

	//UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

	//if (PBViewport != NULL)
	//{
	//	PBViewport->ShowLoadingScreen();
	//	PBViewport->SetUpMoviePlayerLoadingScreen();
	//}
}

APBGameSession* UPBGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return Cast<APBGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

void UPBGameInstance::TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString)
{
	APBPlayerController_Menu* const FirstPC = Cast<APBPlayerController_Menu>(UPBGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (FirstPC != nullptr)
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Join Session failed.");
		if (ReasonString.IsEmpty() == false)
		{
			ReturnReason = FText::Format(NSLOCTEXT("NetworkErrors", "JoinSessionFailedReasonFmt", "Join Session failed. {0}"), FText::FromString(ReasonString));
		}

		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
	}
}

void UPBGameInstance::ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString)
{
	ShowMessageThenGotoState(Message, OKButtonString, CancelButtonString, PBGameInstanceState::MainMenu);
}

void UPBGameInstance::GotoState(FName NewState)
{
	UE_LOG(LogOnline, Log, TEXT("GotoState: NewState: %s"), *NewState.ToString());

	PendingState = NewState;
}

void UPBGameInstance::MaybeChangeState()
{
	if ((PendingState != CurrentState) && (PendingState != PBGameInstanceState::None))
	{
		//StartFadeOutScreen();

		// end current state
		EndCurrentState();

		// begin new state
		BeginNewState(PendingState);

		// clear pending change
		PendingState = PBGameInstanceState::None;

		//if (OldState == PBGameInstanceState::None)
		//{
		//	BeginNewStateFunc(true);
		//}
		//else
		//{
		//	auto SFIO = GetWidgetScreenFadeInOut();
		//	if (SFIO && SFIO->IsFadingOut())
		//	{
		//		SFIO->OnFinishToFade.AddLambda(BeginNewStateFunc);
		//	}
		//}
	}
}

void UPBGameInstance::EndCurrentState()
{
	PreviousState = CurrentState;

	// per-state custom ending code here
	if (CurrentState == PBGameInstanceState::PendingInvite)
	{
		EndPendingInviteState();
	}
	else if (CurrentState == PBGameInstanceState::Login)
	{
		EndLoginState();
	}
	else if (CurrentState == PBGameInstanceState::MainMenu)
	{
		EndMainMenuState();
	}
	else if (CurrentState == PBGameInstanceState::MessageMenu)
	{
		EndMessageMenuState();
	}
	else if (CurrentState == PBGameInstanceState::LoadingScreen)
	{
		EndLoadingScreenState();
	}
	else if (CurrentState == PBGameInstanceState::Playing)
	{
		EndPlayingState();
	}

	CurrentState = PBGameInstanceState::None;
}

void UPBGameInstance::BeginNewState(FName NewState)
{
	// per-state custom starting code here
	CurrentState = NewState;

	if (NewState == PBGameInstanceState::PendingInvite)
	{
		BeginPendingInviteState();
	}
	else if (NewState == PBGameInstanceState::Login)
	{
		BeginLoginState();
	}
	else if (NewState == PBGameInstanceState::MainMenu)
	{
		BeginMainMenuState();
	}
	else if (NewState == PBGameInstanceState::MessageMenu)
	{
		BeginMessageMenuState();
	}
	else if (NewState == PBGameInstanceState::LoadingScreen)
	{
		BeginLoadingScreenState();
	}
	else if (NewState == PBGameInstanceState::Playing)
	{
		BeginPlayingState();
	}	
}

void UPBGameInstance::BeginPendingInviteState()
{
	if (LoadFrontEndMap(MainMenuMap))
	{
		StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UPBGameInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
	}
	else
	{
		GotoState(PBGameInstanceState::Login);
	}
}

void UPBGameInstance::EndPendingInviteState()
{
	// cleanup in case the state changed before the pending invite was handled.
	CleanupOnlinePrivilegeTask();
}

void UPBGameInstance::BeginLoginState()
{
	SetIsOnline(false);

	CreateWidgetSystemMessageListBox();

	LoadLoginMap();
}

void UPBGameInstance::EndLoginState()
{
	//StartFadeInScreen();

	if (LoginUI)
	{
		LoginUI->Hide();
		LoginUI = nullptr;
	}
}	

bool UPBGameInstance::LoadLoginMap()
{
	PendingStream.LevelName = LoginMap;
	PendingStream.CallbackFuncOnLoaded = TEXT("OnPostLoadLoginMap");
	PendingStream.bLevelVisibleOnLoaded = false;

	return LoadFrontEndMap(LoginMap);
}

void UPBGameInstance::OnPostLoadLoginMap()
{
	UE_LOG(LogOnline, Log, TEXT("OnPostLoadLoginMap"), *PendingStream.LevelName);
	SetCurrentStreamingLevelName(PendingStream.LevelName);
	ShowStreamingLevel(PendingStream.LevelName);

	ULocalPlayer* const Player = GetFirstGamePlayer();
	ensure(Player && Player->PlayerController);

	ensure(LoginClass);
	LoginUI = CreateWidget<UPBLoginWidget>(Player->PlayerController, LoginClass);
	LoginUI->Show();
	UPBUserWidget::NavigateUserFocus(Player, LoginUI);

	FInputModeUIOnly InputMode;
	Player->PlayerController->SetInputMode(InputMode);
	LoginUI->SetKeyboardFocus();
	LoginUI->SetUserFocus(Player->PlayerController);
}

void UPBGameInstance::SetPresenceForLocalPlayers(const FVariantData& PresenceData)
{
	const auto Presence = Online::GetPresenceInterface();
	if (Presence.IsValid())
	{
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			const TSharedPtr<const FUniqueNetId> UserId = LocalPlayers[i]->GetPreferredUniqueNetId();

			if (UserId.IsValid())
			{
				FOnlineUserPresenceStatus PresenceStatus;
				PresenceStatus.Properties.Add(DefaultPresenceKey, PresenceData);

				Presence->SetPresence(*UserId, PresenceStatus);
			}
		}
	}
}

void UPBGameInstance::BeginMainMenuState()
{
	//StartFadeInScreen();

	// Make sure we're not showing the loadscreen
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

	if (PBViewport != NULL)
	{
		PBViewport->HideLoadingScreen();
	}

	SetIsOnline(false);

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

	// load startup map
	LoadLobbyMap();

	//BuildLobbyInstance();

	RemoveNetworkFailureHandlers();

	PlayBGM();
}

void UPBGameInstance::EndMainMenuState()
{
	//StartFadeOutScreen();

	StopBGM();
}

void UPBGameInstance::BeginMessageMenuState()
{
	if (PendingMessage.DisplayString.IsEmpty())
	{
		UE_LOG(LogOnlineGame, Warning, TEXT("UPBGameInstance::BeginMessageMenuState: Display string is empty"));
		GotoInitialState();
		return;
	}

	// Make sure we're not showing the loadscreen
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

	if (PBViewport != NULL)
	{
		PBViewport->HideLoadingScreen();
	}

	check(!MessageMenuUI.IsValid());
	MessageMenuUI = MakeShareable(new FPBMessageMenu);
	MessageMenuUI->Construct(this, PendingMessage.PlayerOwner, PendingMessage.DisplayString, PendingMessage.OKButtonString, PendingMessage.CancelButtonString, PendingMessage.NextState);

	PendingMessage.DisplayString = FText::GetEmpty();
}

void UPBGameInstance::BeginLoadingScreenState()
{
	if (LobbyInstance)
	{
		UPBWidgetPage* CurrentPage = LobbyInstance->GetCurrentPage();
		if (CurrentPage)
		{
			CurrentPage->Exit();
		}
	}
}

void UPBGameInstance::EndMessageMenuState()
{
	if (MessageMenuUI.IsValid())
	{
		MessageMenuUI->RemoveFromGameViewport();
		MessageMenuUI = nullptr;
	}
}

void UPBGameInstance::EndLoadingScreenState()
{

}

void UPBGameInstance::BeginPlayingState()
{
	// Set presence for playing in a map
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("InGame"))));

	// Make sure viewport has focus
	FSlateApplication::Get().SetAllUserFocusToGameViewport();

	//StartFadeInScreen(5);
}

void UPBGameInstance::EndPlayingState()
{
	//StartFadeOutScreen();

	// Clear the players' presence information
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));
}

void UPBGameInstance::LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const
{
	APBPlayerState* const PlayerState = LocalPlayer && LocalPlayer->PlayerController ? Cast<APBPlayerState>(LocalPlayer->PlayerController->PlayerState) : nullptr;
	if (PlayerState)
	{
		PlayerState->SetQuitter(true);
	}
}

void UPBGameInstance::RemoveNetworkFailureHandlers()
{
	// Remove the local session/travel failure bindings if they exist
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(TravelLocalSessionFailureDelegateHandle);
	}
}

void UPBGameInstance::AddNetworkFailureHandlers()
{
	// Add network/travel error handlers (if they are not already there)
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UPBGameInstance::TravelLocalSessionFailure);
	}
}

bool UPBGameInstance::ReturnToMainMenu(bool UnexpectedFromClient)
{
	if (CurrentState != PBGameInstanceState::Playing)
		return false;

	UWorld* World = GetWorld();
	if (World)
	{
		APBGameMode* GM = Cast<APBGameMode>(World->GetAuthGameMode());
		if (GM)
		{
			GM->HandleReturnToMainMenu(UnexpectedFromClient);
		}

		GotoState(PBGameInstanceState::MainMenu);
		return true;
	}

	return false;
}

bool UPBGameInstance::LoadLobbyMap()
{
	PendingStream.LevelName = MainMenuMap;
	PendingStream.CallbackFuncOnLoaded = TEXT("OnPostLoadLobbyMap");
	PendingStream.bLevelVisibleOnLoaded = true;

	return LoadFrontEndMap(MainMenuMap);
}

//Load
void UPBGameInstance::OnPostLoadLobbyMap()
{
	UE_LOG(LogOnline, Log, TEXT("OnPostLoadLobbyMap %s"), *PendingStream.LevelName);
	SetCurrentStreamingLevelName(PendingStream.LevelName);
	ShowStreamingLevel(PendingStream.LevelName);

	BuildLobbyInstance();

	if (LobbyInstance)
	{
		LobbyInstance->GoToPage(EPageType::MainMenu);
	}

	// 이전맵은 언로드한다.
	if (PreviousStreamingLevelName != CurrentStreamingLevelName)
	{
		UnloadStreamingLevel(PreviousStreamingLevelName);
	}

	//////////////////////////////////////////////////////////////////////////
	// for testing
	if (!PendingGameURL.IsEmpty() && !PendingGameMap.IsEmpty())
	{
		MininumLoadingScreenDuration = 5.f;
		const float PendingEnterGameTime = 5.f;
		FTimerHandle TimerHandle_PendingEnterGame;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PendingEnterGame, this, &UPBGameInstance::PendingEnterGame, PendingEnterGameTime, false);
	}
	//~ for testing
	//////////////////////////////////////////////////////////////////////////
}

bool UPBGameInstance::LoadFrontEndMap(const FString& MapName)
{
	bool bSuccess = true;

	// if already loaded, Only call functions
	UWorld* const World = GetWorld();
	if (World)
	{
		ULevelStreaming* StreamingLevel = GetStreamingLevel(MapName);
		if (StreamingLevel && StreamingLevel->bShouldBeLoaded)
		{
			if (PreviousState == PBGameInstanceState::Login && CurrentState == PBGameInstanceState::MainMenu)
			{
				OnPostLoadLobbyMap();
				return bSuccess;
			}
		}

		FString const RootLevelName = *World->PersistentLevel->GetOutermost()->GetName();
		if (RootLevelName == StreamingRootMap)
		{
			if (PreviousState == PBGameInstanceState::Login && CurrentState == PBGameInstanceState::MainMenu)
			{
				OnPostLoadMap();
				return bSuccess;
			}
		}
	}

	FString Error;
	EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
	FString ModeShortName = GetWorld()->IsPlayInEditor() ? TEXT("") : TEXT("LOBBY");
	FString SpecificMapName = GetWorld()->IsPlayInEditor() ? MapName : StreamingRootMap;
	FString TravelURL = FString::Printf(TEXT("%s?game=%s"), *SpecificMapName, *ModeShortName);

	FURL DefaultURL;
	FURL URL(&DefaultURL, *TravelURL, ETravelType::TRAVEL_Partial);

	if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject)) //CastChecked<UEngine>() will fail if using Default__PBGameInstance, so make sure that we're not default
	{
		BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);

		// Handle failure.
		if (BrowseRet != EBrowseReturnVal::Success)
		{
			UE_LOG(LogLoad, Fatal, TEXT("Failed to enter Root : %s(StreamLevel : %s) : %s.Please check the log for errors."), *StreamingRootMap, *MapName, *Error);
			bSuccess = false;
		}
	}
	return bSuccess;
}

void UPBGameInstance::OnPreLoadMap(const FString& MapName)
{
}

void UPBGameInstance::OnPostLoadMap()
{
	if (GetWorld())
	{
		if (CurrentState == PBGameInstanceState::LoadingScreen && bLoadingScreenFinished)
		{
			return;
		}

		FString const LevelName = *GetWorld()->PersistentLevel->GetOutermost()->GetName();
		const bool bRootLevel = LevelName.Contains(TEXT("Root"));
		const bool HasStreamingLevel = GetWorld()->StreamingLevels.Num() > 0;

		if (HasStreamingLevel && bRootLevel)
		{
			LoadStreamingLevel(PendingStream);
		}
	}
}

void UPBGameInstance::UseOnceSimpleLoadMap()
{
	UPBEngine* PBEngine = Cast<UPBEngine>(GEngine);
	if (PBEngine)
	{
		PBEngine->bOnceSimpleLoadMap = true;
	}
}

bool UPBGameInstance::DelayPendingNetGameTravel()
{
	return Super::DelayPendingNetGameTravel() || (CurrentState == PBGameInstanceState::LoadingScreen && bDelayPendingNetGameTravel);
}

FString UPBGameInstance::GetMapNameFromTable(uint32 MapIdx)
{
	UPBMapTable* MapTable = Cast<UPBMapTable>(UPBGameplayStatics::GetGameTableManager(this)->GetTable(EPBTableType::GameMap));
	if (MapTable)
	{
		FPBMapTableData* Data = MapTable->GetData(MapIdx);
		if (Data && Data->BPClass)
		{
			FString MapName = Data->BPClass.GetLongPackageName();
			return MapName;
		}
	}
	return TEXT("");
}

FString UPBGameInstance::GetFirstGameStreamingLevel()
{
	if (GetWorld())
	{
		for (ULevelStreaming* Level : GetWorld()->StreamingLevels)
		{
			if (Level)
			{
				FString StreamLevelName = Level->PackageNameToLoad.IsNone() ? Level->GetWorldAssetPackageFName().ToString() : Level->PackageNameToLoad.ToString();
				if (StreamLevelName.IsEmpty()) continue;

				// 로비맵이면 패스
				if (MainMenuMap.Contains(StreamLevelName) || StreamLevelName.Contains(MainMenuMap))
				{
					continue;
				}

				return StreamLevelName;
			}
		}
	}
	return TEXT("");
}

bool UPBGameInstance::EnterGame(FString DediURL, uint32 MapIdx)
{
	// DediURL includes IP / Port / GameMode

	FString MapName = GetMapNameFromTable(MapIdx);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (MapName.IsEmpty() || !TestGameMap.IsEmpty())
	{
		MapName = TestGameMap;
		UE_LOG(LogOnline, Warning, TEXT("Can't find MapName from 'Content/Table/tbMaps' (MapIdx:%d) or TestGameMap is set, so apply the TestGameMap of DefaultGame.ini"), MapIdx);
	}
#endif

	if (!MapName.IsEmpty())
	{
		// 설정한 맵이 스트리밍 레벨에 등록 되어있는지 최종 확인
		ensureMsgf(GetStreamingLevel(MapName), *FString::Printf(TEXT("Can't find streaming level(%s) in 'Content/Maps/StreamingRootLevel'"), *MapName));

		PendingStream.LevelName = MapName;
		PendingStream.CallbackFuncOnLoaded = TEXT("Client_OnPostLoadPlayMap");
		PendingStream.bLevelVisibleOnLoaded = false;

		return EnterGameInternal(DediURL);
	}

	UE_LOG(LogOnline, Warning, TEXT("Can't find MapName from 'Content/Table/tbMaps' (MapIdx:%d)"), MapIdx);
	return false;
}

void UPBGameInstance::PendingEnterGame()
{
	if (!PendingGameMap.IsEmpty())
	{
		PendingStream.LevelName = PendingGameMap;
		PendingStream.CallbackFuncOnLoaded = TEXT("Client_OnPostLoadPlayMap");
		PendingStream.bLevelVisibleOnLoaded = false;

		EnterGameInternal(PendingGameURL);

		PendingGameURL.Empty();
		PendingGameMap.Empty();
	}
}

bool UPBGameInstance::EnterGameInternal(FString TravelURL)
{
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	if (PlayerController == nullptr)
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "InvalidPlayerController", "Invalid Player Controller");
		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
		return false;
	}
	
	AddNetworkFailureHandlers();
	const bool bNeedsStreamingLevelLoad = !PendingStream.LevelName.IsEmpty();
	GotoState(bNeedsStreamingLevelLoad ? PBGameInstanceState::LoadingScreen : PBGameInstanceState::Playing);	

	StartLoadingScreen();

	// 서버와 맵 동기화를 시작하기전에 클라이언트에서 먼저 로딩을 시작한다.
	LoadStreamingLevel(PendingStream);

	// Net 연결은 미리 해놓고 클라이언트가 맵로딩이 완료될 때까지 서버와의 맵 동기화를 지연시킨다.
	UseOnceSimpleLoadMap();
	bDelayPendingNetGameTravel = true;
	PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);

	return true;
}

void UPBGameInstance::HandleEnterGameCountdownStarted()
{
	if (LobbyInstance && LobbyInstance->GetCharacterScene())
	{
		LobbyInstance->GetCharacterScene()->PendingSpawnTeamCharacters(0.3f, false, false);
		LobbyInstance->TourCamFromTo(FLobbyCamName::Multiplay, FLobbyCamName::LoadingScreen);
	}
}

void UPBGameInstance::StartLoadingScreen()
{
	LoadingScreenStartTime = GetWorld()->GetTimeSeconds();
	bLoadingScreenFinished = false;	

	if (LobbyInstance && LobbyInstance->GetCharacterScene())
	{
		LobbyInstance->GetCharacterScene()->FlushSpawnTeamCharacters(true, true);
	}
}

ULevelStreaming* UPBGameInstance::LoadStreamingLevel(FPendingStreamLevel& InPendingStream)
{
	FString const RootLevelName = GetWorld() ? *GetWorld()->PersistentLevel->GetOutermost()->GetName() : TEXT("");
	if (InPendingStream.LevelName == RootLevelName || InPendingStream.LevelName == CurrentStreamingLevelName)
	{
		return nullptr;
	}

	ULevelStreaming* Level = GetStreamingLevel(InPendingStream.LevelName);
	if (Level)
	{
		PreviousStreamingLevelName = CurrentStreamingLevelName;

		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = *InPendingStream.CallbackFuncOnLoaded; //must be sure to make it BlueprintCallable
		LatentInfo.Linkage = 1;
		UGameplayStatics::LoadStreamLevel(this, *InPendingStream.LevelName, InPendingStream.bLevelVisibleOnLoaded, true, LatentInfo);

		UE_LOG(LogOnline, Log, TEXT("map load start(%s)"), *InPendingStream.LevelName);
		return Level;
	}

	UE_LOG(LogOnline, Warning, TEXT("Can't find child streaming level (%s) from 'StreamingLevelRoot, you should check child level in detail pannel of 'Content/Maps/StreamingRoolLevel'"), *InPendingStream.LevelName);
	return Level;
}

void UPBGameInstance::UnloadStreamingLevel(FString MapName, FString CallbackFunction)
{
	if (MapName == CurrentStreamingLevelName)
	{
		return;
	}

	ULevelStreaming* Level = GetStreamingLevel(MapName);
	if (Level && Level->bShouldBeLoaded)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = *CallbackFunction; //be sure to make it BlueprintCallable
		LatentInfo.Linkage = 1;
		UGameplayStatics::UnloadStreamLevel(this, *MapName, LatentInfo);

		UE_LOG(LogOnline, Log, TEXT("Unload map(%s)"), *MapName);
	}
}

ULevelStreaming* UPBGameInstance::GetStreamingLevel(FString MapName)
{
	if (GetWorld() && !MapName.IsEmpty())
	{
		for (ULevelStreaming* Level : GetWorld()->StreamingLevels)
		{
			if (Level)
			{
				FString StreamLevelName = Level->PackageNameToLoad.IsNone() ? Level->GetWorldAssetPackageFName().ToString() : Level->PackageNameToLoad.ToString();
				if ((StreamLevelName.Contains(MapName) || MapName.Contains(StreamLevelName)))
				{
					return Level;
				}
			}
		}
	}

	return nullptr;
}

void UPBGameInstance::ShowStreamingLevel(FString MapName)
{
	ULevelStreaming* Level = GetStreamingLevel(MapName);
	if (Level && Level->IsLevelLoaded() && !Level->bShouldBeVisible)
	{
		Level->bShouldBeVisible = true;

		if (GetWorld())
		{
			GetWorld()->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);
		}
	}
}

void UPBGameInstance::SetCurrentStreamingLevelName(FString MapName)
{
	PreviousStreamingLevelName = CurrentStreamingLevelName;
	CurrentStreamingLevelName = MapName;
}

//Load
void UPBGameInstance::Client_OnPostLoadPlayMap()
{
	UE_LOG(LogOnline, Log, TEXT("client map loaded(%s)"), *PendingStream.LevelName);
	//SetCurrentStreamingLevelName(PendingStream.LevelName); --> OnLoadingScreenFinished 에서 설정함

	if (GetWorld())
	{
		// 로딩이 완료되었더라도 로딩스크린 화면을 최소 몇 초 이상 유지한다.
		float LoadingElapsedTime = GetWorld()->GetTimeSeconds() - LoadingScreenStartTime;
		float RemainingDuration = FMath::Max(MininumLoadingScreenDuration - LoadingElapsedTime, 0.f);
		if (RemainingDuration > 0.f)
		{
			FTimerHandle TimerHandle_PendingGame;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_PendingGame, this, &UPBGameInstance::OnLoadingScreenFinished, RemainingDuration, false);
		}
		else
		{
			OnLoadingScreenFinished();
		}
	}
}

void UPBGameInstance::OnLoadingScreenFinished()
{
	float LoadingElapsedTime = GetWorld()->GetTimeSeconds() - LoadingScreenStartTime;
	UE_LOG(LogOnline, Log, TEXT("Loading Scene completed(Elapsed:%f)"), LoadingElapsedTime);
	SetCurrentStreamingLevelName(PendingStream.LevelName);

	PendingStream.Cleanup();
	
	GotoState(PBGameInstanceState::Playing);

	bLoadingScreenFinished = true;

	// Lobby Level 에 있는 무기를 다음 level 로 넘어가기전에 미리 삭제한다.
	for (FActorIterator ActorIt(GetWorld()); ActorIt; ++ActorIt)
	{
		ActorIt->RouteEndPlay(EEndPlayReason::RemovedFromWorld);

		APBWeapon* Weapon = Cast<APBWeapon>(*ActorIt);
		if (Weapon)
		{
			Weapon->SetActorHiddenInGame(true);
			GetWorld()->DestroyActor(Weapon, true);
		}
	}

	// Lobby character 삭제
	if (LobbyInstance && LobbyInstance->GetCharacterScene())
	{
		LobbyInstance->GetCharacterScene()->DestroySpawnedCharacters();
	}

	// LobbyMap 은 사라지게한다.
	UnloadStreamingLevel(PreviousStreamingLevelName, TEXT("OnPostUnloadLobbyMap"));	
}

//Unload
void UPBGameInstance::OnPostUnloadLobbyMap()
{
	UE_LOG(LogOnline, Log, TEXT("OnPostUnloadLobbyMap %s"), *PreviousStreamingLevelName);

	// PlayMap 를 보이게한다.	
	//ShowStreamingLevel(CurrentStreamingLevelName);	

	// 서버와 맵 동기화를 시작한다.
	bDelayPendingNetGameTravel = false;
}

void UPBGameInstance::Server_OnPostLoadPlayMap()
{	
	UE_LOG(LogOnline, Log, TEXT("server map loaded(%s)"), *PendingStream.LevelName);
	SetCurrentStreamingLevelName(PendingStream.LevelName);

	if (IsSpawnedByDediManager())
	{
		DediHandleGameReady_Handle = OnGameReadyEvt.AddUObject(this, &UPBGameInstance::DediHandleGameReady);
	}

	GotoState(PBGameInstanceState::Playing);
}

bool UPBGameInstance::Tick(float DeltaSeconds)
{
	if (PacketManager)
	{
		PacketManager->Update();
	}

	// Dedicated server doesn't need to worry about game state
	if (IsRunningDedicatedServer() == true)
	{
		return true;
	}

	MaybeChangeState();

	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

	if (CurrentState != PBGameInstanceState::Login)
	{
		// If at any point we aren't licensed (but we are after welcome screen) bounce them back to the welcome screen
		if (!bIsLicensed && CurrentState != PBGameInstanceState::None && !PBViewport->IsShowingDialog())
		{
			const FText ReturnReason = NSLOCTEXT("ProfileMessages", "NeedLicense", "The signed in users do not have a license for this game. Please purchase PBGame from the Xbox Marketplace or sign in a user with a valid license.");
			const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

			ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), PBGameInstanceState::Login);
		}

		// Show controller disconnected dialog if any local players have an invalid controller
		if (PBViewport != NULL &&
			!PBViewport->IsShowingDialog())
		{
			for (int i = 0; i < LocalPlayers.Num(); ++i)
			{
				if (LocalPlayers[i] && LocalPlayers[i]->GetControllerId() == -1)
				{
					PBViewport->ShowDialog(
						LocalPlayers[i],
						EPBDialogType::ControllerDisconnected,
						FText::Format(NSLOCTEXT("ProfileMessages", "PlayerReconnectControllerFmt", "Player {0}, please reconnect your controller."), FText::AsNumber(i + 1)),
#ifdef PLATFORM_PS4
						NSLOCTEXT("DialogButtons", "PS4_CrossButtonContinue", "Cross Button - Continue"),
#else
						NSLOCTEXT("DialogButtons", "AButtonContinue", "A - Continue"),
#endif
						FText::GetEmpty(),
						FOnClicked::CreateUObject(this, &UPBGameInstance::OnControllerReconnectConfirm),
						FOnClicked()
					);
				}
			}
		}
	}

	// If we have a pending invite, and the session is properly shut down, accept it
	if (PendingInvite.UserId.IsValid() && PendingInvite.bPrivilegesCheckedAndAllowed && CurrentState == PBGameInstanceState::PendingInvite)
	{
		IOnlineSubsystem * OnlineSub = IOnlineSubsystem::Get();
		IOnlineSessionPtr Sessions = (OnlineSub != NULL) ? OnlineSub->GetSessionInterface() : NULL;

		if (Sessions.IsValid())
		{
			EOnlineSessionState::Type SessionState = Sessions->GetSessionState(GameSessionName);

			if (SessionState == EOnlineSessionState::NoSession)
			{
				ULocalPlayer * NewPlayerOwner = GetFirstGamePlayer();

				if (NewPlayerOwner != nullptr)
				{
					NewPlayerOwner->SetControllerId(PendingInvite.ControllerId);
					NewPlayerOwner->SetCachedUniqueNetId(PendingInvite.UserId);
					SetIsOnline(true);
					JoinSession(NewPlayerOwner, PendingInvite.InviteResult);
				}

				PendingInvite.UserId.Reset();
			}
		}
	}

	return true;
}

bool UPBGameInstance::HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld)
{
	bool const bOpenSuccessful = Super::HandleOpenCommand(Cmd, Ar, InWorld);
	if (bOpenSuccessful)
	{
		GotoState(PBGameInstanceState::Playing);
	}
	return bOpenSuccessful;
}

void UPBGameInstance::HandleSignInChangeMessaging()
{
	// Master user signed out, go to initial state (if we aren't there already)
	if (CurrentState != GetInitialState())
	{
#if PB_CONSOLE_UI
		// Display message on consoles
		const FText ReturnReason = NSLOCTEXT("ProfileMessages", "SignInChange", "Sign in status change occurred.");
		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

		ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), GetInitialState());
#else								
		GotoInitialState();
#endif
	}
}

void UPBGameInstance::HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId)
{
	const bool bDowngraded = (LoginStatus == ELoginStatus::NotLoggedIn && !GetIsOnline()) || (LoginStatus != ELoginStatus::LoggedIn && GetIsOnline());

	UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: bDownGraded: %i"), (int)bDowngraded);

	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();

	// Find the local player associated with this unique net id
	ULocalPlayer * LocalPlayer = FindLocalPlayerFromUniqueNetId(UserId);

	// If this user is signed out, but was previously signed in, punt to welcome
	if (LocalPlayer != NULL)
	{
		if (bDowngraded)
		{
			UE_LOG(LogOnline, Log, TEXT("HandleUserLoginChanged: Player logged out: %s"), *UserId.ToString());

			LabelPlayerAsQuitter(LocalPlayer);

			if (LocalPlayer == GetFirstGamePlayer() || GetIsOnline())
			{
				HandleSignInChangeMessaging();
			}
		}
	}
}

void UPBGameInstance::HandleAppWillDeactivate()
{
	if (CurrentState == PBGameInstanceState::Playing)
	{
		// Just have the first player controller pause the game.
		UWorld* const GameWorld = GetWorld();
		if (GameWorld)
		{
			// protect against a second pause menu loading on top of an existing one if someone presses the Jewel / PS buttons.
			bool bNeedsPause = true;
			for (FConstControllerIterator It = GameWorld->GetControllerIterator(); It; ++It)
			{
				APBPlayerController* Controller = Cast<APBPlayerController>(*It);
				if (Controller && (Controller->IsPaused() || Controller->IsGameMenuVisible()))
				{
					bNeedsPause = false;
					break;
				}
			}

			if (bNeedsPause)
			{
				APBPlayerController* const Controller = Cast<APBPlayerController>(GameWorld->GetFirstPlayerController());
				if (Controller)
				{
					Controller->ShowInGameMenu();
				}
			}
		}
	}
}

void UPBGameInstance::HandleAppSuspend()
{
	// Players will lose connection on resume. However it is possible the game will exit before we get a resume, so we must kick off round end events here.
	UE_LOG(LogOnline, Warning, TEXT("UPBGameInstance::HandleAppSuspend"));
	UWorld* const World = GetWorld();
	APBGameState* const GameState = World != NULL ? World->GetGameState<APBGameState>() : NULL;

	if (CurrentState != PBGameInstanceState::None && CurrentState != GetInitialState())
	{
		UE_LOG(LogOnline, Warning, TEXT("UPBGameInstance::HandleAppSuspend: Sending round end event for players"));
	}
}

void UPBGameInstance::HandleAppResume()
{
	UE_LOG(LogOnline, Log, TEXT("UPBGameInstance::HandleAppResume"));

	if (CurrentState != PBGameInstanceState::None && CurrentState != GetInitialState())
	{
		UE_LOG(LogOnline, Warning, TEXT("UPBGameInstance::HandleAppResume: Attempting to sign out players"));

		for (int32 i = 0; i < LocalPlayers.Num(); ++i)
		{
			if (LocalPlayers[i]->GetCachedUniqueNetId().IsValid() && !IsLocalPlayerOnline(LocalPlayers[i]))
			{
				UE_LOG(LogOnline, Log, TEXT("UPBGameInstance::HandleAppResume: Signed out during resume."));
				HandleSignInChangeMessaging();
				break;
			}
		}
	}
}

void UPBGameInstance::HandleAppTerminate()
{
}

void UPBGameInstance::HandleAppLicenseUpdate()
{
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();
}

void UPBGameInstance::HandleSafeFrameChanged()
{
	UCanvas::UpdateAllCanvasSafeZoneData();
}

FReply UPBGameInstance::OnPairingUsePreviousProfile()
{
	// Do nothing (except hide the message) if they want to continue using previous profile
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

	if (PBViewport != nullptr)
	{
		PBViewport->HideDialog();
	}

	return FReply::Handled();
}

FReply UPBGameInstance::OnPairingUseNewProfile()
{
	HandleSignInChangeMessaging();
	return FReply::Handled();
}

void UPBGameInstance::HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser)
{
	UE_LOG(LogOnlineGame, Log, TEXT("UPBGameInstance::HandleControllerPairingChanged GameUserIndex %d PreviousUser '%s' NewUser '%s'"),
		GameUserIndex, *PreviousUser.ToString(), *NewUser.ToString());

	if (CurrentState == PBGameInstanceState::Login)
	{
		// Don't care about pairing changes at welcome screen
		return;
	}

#if PB_CONSOLE_UI && PLATFORM_XBOXONE
	if (IgnorePairingChangeForControllerId != -1 && GameUserIndex == IgnorePairingChangeForControllerId)
	{
		// We were told to ignore
		IgnorePairingChangeForControllerId = -1;	// Reset now so there there is no chance this remains in a bad state
		return;
	}

	if (PreviousUser.IsValid() && !NewUser.IsValid())
	{
		// Treat this as a disconnect or signout, which is handled somewhere else
		return;
	}

	if (!PreviousUser.IsValid() && NewUser.IsValid())
	{
		// Treat this as a signin
		ULocalPlayer * ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

		if (ControlledLocalPlayer != NULL && !ControlledLocalPlayer->GetCachedUniqueNetId().IsValid())
		{
			// If a player that previously selected "continue without saving" signs into this controller, move them back to welcome screen
			HandleSignInChangeMessaging();
		}

		return;
	}

	// Find the local player currently being controlled by this controller
	ULocalPlayer * ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

	// See if the newly assigned profile is in our local player list
	ULocalPlayer * NewLocalPlayer = FindLocalPlayerFromUniqueNetId(NewUser);

	// If the local player being controlled is not the target of the pairing change, then give them a chance 
	// to continue controlling the old player with this controller
	if (ControlledLocalPlayer != nullptr && ControlledLocalPlayer != NewLocalPlayer)
	{
		UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

		if (PBViewport != nullptr)
		{
			PBViewport->ShowDialog(
				nullptr,
				EPBDialogType::Generic,
				NSLOCTEXT("ProfileMessages", "PairingChanged", "Your controller has been paired to another profile, would you like to switch to this new profile now? Selecting YES will sign out of the previous profile."),
				NSLOCTEXT("DialogButtons", "YES", "A - YES"),
				NSLOCTEXT("DialogButtons", "NO", "B - NO"),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnPairingUseNewProfile),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnPairingUsePreviousProfile)
			);
		}
	}
#endif
}

void UPBGameInstance::HandleLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
}

void UPBGameInstance::HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex)
{
#if PLATFORM_PS4 || PLATFORM_XBOXONE

	UE_LOG(LogOnlineGame, Log, TEXT("UPBGameInstance::HandleControllerConnectionChange bIsConnection %d GameUserIndex %d"),
		bIsConnection, GameUserIndex);

	if (!bIsConnection)
	{
		// Controller was disconnected

		// Find the local player associated with this user index
		ULocalPlayer * LocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

		if (LocalPlayer == NULL)
		{
			return;		// We don't care about players we aren't tracking
		}

		// Invalidate this local player's controller id.
		LocalPlayer->SetControllerId(-1);
	}

#endif
}

FReply UPBGameInstance::OnControllerReconnectConfirm()
{
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());
	if (PBViewport)
	{
		PBViewport->HideDialog();
	}

	return FReply::Handled();
}

TSharedPtr< const FUniqueNetId > UPBGameInstance::GetUniqueNetIdFromControllerId(const int ControllerId)
{
	IOnlineIdentityPtr OnlineIdentityInt = Online::GetIdentityInterface();

	if (OnlineIdentityInt.IsValid())
	{
		TSharedPtr<const FUniqueNetId> UniqueId = OnlineIdentityInt->GetUniquePlayerId(ControllerId);

		if (UniqueId.IsValid())
		{
			return UniqueId;
		}
	}

	return nullptr;
}

void UPBGameInstance::SetIsOnline(bool bInIsOnline)
{
	bIsOnline = bInIsOnline;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		for (int32 i = 0; i < LocalPlayers.Num(); ++i)
		{
			ULocalPlayer* LocalPlayer = LocalPlayers[i];

			TSharedPtr<const FUniqueNetId> PlayerId = LocalPlayer->GetPreferredUniqueNetId();
			if (PlayerId.IsValid())
			{
				OnlineSub->SetUsingMultiplayerFeatures(*PlayerId, bIsOnline);
			}
		}
	}
}

void UPBGameInstance::SetIgnorePairingChangeForControllerId(const int32 ControllerId)
{
	IgnorePairingChangeForControllerId = ControllerId;
}

bool UPBGameInstance::IsLocalPlayerOnline(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer == NULL)
	{
		return false;
	}
	const auto OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			auto UniqueId = LocalPlayer->GetCachedUniqueNetId();
			if (UniqueId.IsValid())
			{
				const auto LoginStatus = IdentityInterface->GetLoginStatus(*UniqueId);
				if (LoginStatus == ELoginStatus::LoggedIn)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UPBGameInstance::ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer)
{
	// Get the viewport
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());

#if PLATFORM_XBOXONE
	if (CurrentConnectionStatus != EOnlineServerConnectionStatus::Connected)
	{
		// Don't let them play online if they aren't connected to Xbox LIVE
		if (PBViewport != NULL)
		{
			const FText Msg = NSLOCTEXT("NetworkFailures", "ServiceDisconnected", "You must be connected to the Xbox LIVE service to play online.");
			const FText OKButtonString = NSLOCTEXT("DialogButtons", "OKAY", "OK");

			PBViewport->ShowDialog(
				NULL,
				EPBDialogType::Generic,
				Msg,
				OKButtonString,
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}

		return false;
	}
#endif

	if (!IsLocalPlayerOnline(LocalPlayer))
	{
		// Don't let them play online if they aren't online
		if (PBViewport != NULL)
		{
			const FText Msg = NSLOCTEXT("NetworkFailures", "MustBeSignedIn", "You must be signed in to play online");
			const FText OKButtonString = NSLOCTEXT("DialogButtons", "OKAY", "OK");

			PBViewport->ShowDialog(
				NULL,
				EPBDialogType::Generic,
				Msg,
				OKButtonString,
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}

		return false;
	}

	return true;
}

FReply UPBGameInstance::OnConfirmGeneric()
{
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());
	if (PBViewport)
	{
		PBViewport->HideDialog();
	}

	return FReply::Handled();
}

void UPBGameInstance::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId)
{
	WaitDialogUI = CreateWidget<UPBWaitDialogWidget>(this, WaitDialogClass);
	if (WaitDialogUI)
	{
		WaitDialogUI->Init(NSLOCTEXT("NetworkStatus", "CheckingPrivilegesWithServer", "Checking privileges with server.  Please wait..."));
		WaitDialogUI->Show(1000);
	}

	auto Identity = Online::GetIdentityInterface();
	if (Identity.IsValid() && UserId.IsValid())
	{
		Identity->GetUserPrivilege(*UserId, Privilege, Delegate);
	}
	else
	{
		// Can only get away with faking the UniqueNetId here because the delegates don't use it
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
	}
}

void UPBGameInstance::CleanupOnlinePrivilegeTask()
{
	if (WaitDialogUI)
	{
		WaitDialogUI->Hide();
		WaitDialogUI = nullptr;
	}
}

void UPBGameInstance::DisplayOnlinePrivilegeFailureDialogs(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	// Show warning that the user cannot play due to age restrictions
	UPBGameViewportClient * PBViewport = Cast<UPBGameViewportClient>(GetGameViewportClient());
	TWeakObjectPtr<ULocalPlayer> OwningPlayer;
	if (GEngine)
	{
		for (auto It = GEngine->GetLocalPlayerIterator(GetWorld()); It; ++It)
		{
			TSharedPtr<const FUniqueNetId> OtherId = (*It)->GetPreferredUniqueNetId();
			if (OtherId.IsValid())
			{
				if (UserId == (*OtherId))
				{
					OwningPlayer = *It;
				}
			}
		}
	}

	if (PBViewport != NULL && OwningPlayer.IsValid())
	{
		if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AccountTypeFailure) != 0)
		{
			IOnlineExternalUIPtr ExternalUI = Online::GetExternalUIInterface();
			if (ExternalUI.IsValid())
			{
				ExternalUI->ShowAccountUpgradeUI(UserId);
			}
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredSystemUpdate) != 0)
		{
			PBViewport->ShowDialog(
				OwningPlayer.Get(),
				EPBDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "RequiredSystemUpdate", "A required system update is available.  Please upgrade to access online features."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredPatchAvailable) != 0)
		{
			PBViewport->ShowDialog(
				OwningPlayer.Get(),
				EPBDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "RequiredPatchAvailable", "A required game patch is available.  Please upgrade to access online features."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AgeRestrictionFailure) != 0)
		{
			PBViewport->ShowDialog(
				OwningPlayer.Get(),
				EPBDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "AgeRestrictionFailure", "Cannot play due to age restrictions!"),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::UserNotFound) != 0)
		{
			PBViewport->ShowDialog(
				OwningPlayer.Get(),
				EPBDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "UserNotFound", "Cannot play due invalid user!"),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::GenericFailure) != 0)
		{
			PBViewport->ShowDialog(
				OwningPlayer.Get(),
				EPBDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "GenericFailure", "Cannot play online.  Check your network connection."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UPBGameInstance::OnConfirmGeneric)
			);
		}
	}
}

void UPBGameInstance::PlayBGM()
{
	if (LobbyBGM != nullptr)
	{
		if (LobbyBGMComp == nullptr)
		{
			LobbyBGMComp = NewObject<UAudioComponent>(this);
			LobbyBGMComp->bAllowSpatialization = false;
			LobbyBGMComp->bShouldRemainActiveIfDropped = true;
			LobbyBGMComp->SetSound(LobbyBGM);
		}

		if (LobbyBGMComp)
		{
			LobbyBGMComp->Play();
		}
	}
}

void UPBGameInstance::StopBGM()
{
	if (LobbyBGMComp && LobbyBGMComp->IsPlaying() && !LobbyBGMComp->IsPendingKill())
	{
		LobbyBGMComp->Stop();
	}
}

void UPBGameInstance::CreateWidgetSystemMessageListBox()
{
	if (IsDedicatedServerInstance())
		return;

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (nullptr != SystemMessageListBox)
		return;
	
	if (ensure(SystemMessageWidgetClass))
	{
		SystemMessageListBox = CreateWidget<UPBWidget_SystemMessageListBox>(World, SystemMessageWidgetClass);
		SystemMessageListBox->AddToViewport(10);
		SystemMessageListBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		SystemMessageListBox->bIsFocusable = false;
	}
}

void UPBGameInstance::DestroyWidgetSystemMessageListBox()
{
	if (nullptr == SystemMessageListBox)
		return;

	SystemMessageListBox->SetVisibility(ESlateVisibility::Hidden);
	SystemMessageListBox = nullptr;
}

class UPBWidget_SystemMessageListBox *UPBGameInstance::GetWidgetSystemMessageListBox() const
{
	ensure(SystemMessageListBox);
	return SystemMessageListBox;
}

void UPBGameInstance::CreateWidgetScreenFadeInOut()
{
	if (IsDedicatedServerInstance())
		return;

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (nullptr != ScreenFadeInOut)
		return;

	if (ensure(ScreenFadeInOutClass))
	{
		//ScreenFadeInOut = CreateWidget<UPBWidget_ScreenFadeInOut>(World, ScreenFadeInOutClass);
		//ScreenFadeInOut->AddToViewport(9);
		//ScreenFadeInOut->SetVisibility(ESlateVisibility::HitTestInvisible);
		//ScreenFadeInOut->bIsFocusable = false;
		//ScreenFadeInOut->StartToFade(0);
	}
}

void UPBGameInstance::DestroyWidgetScreenFadeInOut()
{
	if (nullptr == ScreenFadeInOut)
		return;

	ScreenFadeInOut->SetVisibility(ESlateVisibility::Hidden);
	ScreenFadeInOut = nullptr;
}
class UPBWidget_ScreenFadeInOut *UPBGameInstance::GetWidgetScreenFadeInOut() const
{
	ensure(ScreenFadeInOut);
	return ScreenFadeInOut;
}

void UPBGameInstance::StartFadeOutScreen(float DelayInSeconds)
{
	auto SFIO = GetWidgetScreenFadeInOut();
	if (nullptr == SFIO)
		return;

	SFIO->Reset(false);
	SFIO->StartToFade(DelayInSeconds);
}

void UPBGameInstance::StartFadeInScreen(float DelayInSeconds)
{
	auto SFIO = GetWidgetScreenFadeInOut();
	if (nullptr == SFIO || false == SFIO->IsValidLowLevel())
		return;

	SFIO->Reset(true);
	SFIO->StartToFade(DelayInSeconds);
}

class UPBLobby* UPBGameInstance::GetLobbyInstance()
{
	if (LobbyInstance)
	{
		return LobbyInstance;
	}
	else
	{
		return BuildLobbyInstance();
	}
}



class UPBLobby* UPBGameInstance::BuildLobbyInstance()
{
	// player 0 gets to own the UI
	ULocalPlayer* const Player = GetFirstGamePlayer();
	ensure(Player);

	if (LobbyClass && Player)
	{
		if (nullptr == LobbyInstance)
		{
			LobbyInstance = NewObject<UPBLobby>(this, LobbyClass);
		}
		if (LobbyInstance)
		{
			LobbyInstance->Init(this, Player->PlayerController);
		}
	}

	return LobbyInstance;

}


