// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	PBEngine.cpp: PBEngine c++ code.
=============================================================================*/

#include "PBGame.h"
#include "PBEngine.h"
#include "PBGameInstance.h"
#include "LoadTimeTracker.h"

#if PLATFORM_WINDOWS
bool bHasBeenChangedTitleDebugInfo = false;
HWND FindTopWindow(int32 pid)
{
	std::pair<HWND, int32> params = { 0, pid };

	// Enumerate the windows using a lambda to process each window
	int bResult = ::EnumWindows([](HWND hwnd, LPARAM lParam) -> int
	{
		auto pParams = (std::pair<HWND, int32>*)(lParam);

		unsigned long processId;
		if (::GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
		{
			// Stop enumerating
			::SetLastError(-1);
			pParams->first = hwnd;
			return 0;
		}

		// Continue enumerating
		return 1;
	}, (LPARAM)&params);

	if (!bResult && ::GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}
#endif

UPBEngine::UPBEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if !UE_BUILD_SHIPPING
#	if PLATFORM_WINDOWS
	if (false == bHasBeenChangedTitleDebugInfo)
	{
		//@note: Check out a process ID from Window, And attach to appropriated process for debugging
		TArray<TCHAR> CurrentWindowTitle;
		FString WindowTitle;

		int32 PID = ::GetCurrentProcessId();
		HWND CurrentHandle = FindTopWindow(PID);
		int32 WindowTextLength = ::GetWindowTextLength(CurrentHandle);

		CurrentWindowTitle.AddZeroed(WindowTextLength + 1);
		::GetWindowText(CurrentHandle, &CurrentWindowTitle[0], WindowTextLength + 1);

		WindowTitle.AppendChars(&CurrentWindowTitle[0], WindowTextLength);
		WindowTitle += TEXT(", PID = ") + FString::FromInt(PID);
		::SetWindowText(CurrentHandle, *WindowTitle);
		bHasBeenChangedTitleDebugInfo = true;
	}
#	endif
#endif
	bOnceSimpleLoadMap = false;
}

void UPBEngine::Init(IEngineLoop* InEngineLoop)
{
	// Note: Lots of important things happen in Super::Init(), including spawning the player pawn in-game and
	// creating the renderer.
	Super::Init(InEngineLoop);
}


void UPBEngine::HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// Only handle failure at this level for game or pending net drivers.
	FName NetDriverName = NetDriver ? NetDriver->NetDriverName : NAME_None; 
	if (NetDriverName == NAME_GameNetDriver || NetDriverName == NAME_PendingNetDriver)
	{
		// If this net driver has already been unregistered with this world, then don't handle it.
		//if (World)
		{
			//UNetDriver * NetDriver = FindNamedNetDriver(World, NetDriverName);
			if (NetDriver)
			{
				switch (FailureType)
				{
					case ENetworkFailure::FailureReceived:
					{
						UPBGameInstance* const PBInstance = Cast<UPBGameInstance>(GameInstance);
						if (PBInstance && NetDriver->GetNetMode() == NM_Client)
						{
							const FText OKButton = NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

							// NOTE - We pass in false here to not override the message if we are already going to the main menu
							// We're going to make the assumption that someone else has a better message than "Lost connection to host" if
							// this is the case
							PBInstance->ShowMessageThenGotoState( FText::FromString(ErrorString), OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu, false );
						}
						break;
					}
					case ENetworkFailure::PendingConnectionFailure:						
					{
						UPBGameInstance* const GI = Cast<UPBGameInstance>(GameInstance);
						if (GI && NetDriver->GetNetMode() == NM_Client)
						{
							const FText OKButton = NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

							// NOTE - We pass in false here to not override the message if we are already going to the main menu
							// We're going to make the assumption that someone else has a better message than "Lost connection to host" if
							// this is the case
							GI->ShowMessageThenGotoState( FText::FromString(ErrorString), OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu, false );
						}
						break;
					}
					case ENetworkFailure::ConnectionLost:						
					case ENetworkFailure::ConnectionTimeout:
					{
						UPBGameInstance* const GI = Cast<UPBGameInstance>(GameInstance);
						if (GI && NetDriver->GetNetMode() == NM_Client)
						{
							const FText ReturnReason	= NSLOCTEXT( "NetworkErrors", "HostDisconnect", "Lost connection to host." );
							const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

							// NOTE - We pass in false here to not override the message if we are already going to the main menu
							// We're going to make the assumption that someone else has a better message than "Lost connection to host" if
							// this is the case
							GI->ShowMessageThenGotoState( ReturnReason, OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu, false );
						}
						break;
					}
					case ENetworkFailure::NetDriverAlreadyExists:
					case ENetworkFailure::NetDriverCreateFailure:
					case ENetworkFailure::OutdatedClient:
					case ENetworkFailure::OutdatedServer:
					default:
						break;
				}
			}
		}
	}

	// standard failure handling.
	Super::HandleNetworkFailure(World, NetDriver, FailureType, ErrorString);
}

bool UPBEngine::LoadMap(FWorldContext& WorldContext, FURL URL, class UPendingNetGame* Pending, FString& Error)
{
	if (bOnceSimpleLoadMap)
	{
		// �ΰ��� ����� Ŭ���̾�Ʈ�� �̸� ���� �ε��� ���� ���¶�� �ٸ� �ʿ��� �͵鸸 �߷��� �����Ѵ�.
		bOnceSimpleLoadMap = false;
		return SimpleLoadMap(WorldContext, URL, Pending, Error);
	}
	else
	{
		bOnceSimpleLoadMap = false;
		return Super::LoadMap(WorldContext, URL, Pending, Error);
	}
}

bool UPBEngine::SimpleLoadMap(FWorldContext& WorldContext, FURL URL, class UPendingNetGame* Pending, FString& Error)
{
	STAT_ADD_CUSTOMMESSAGE_NAME(STAT_NamedMarker, *(FString(TEXT("LoadMap - ") + URL.Map)));

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UEngine::LoadMap"), STAT_LoadMap, STATGROUP_LoadTime);

	//NETWORK_PROFILER(GNetworkProfiler.TrackSessionChange(true, URL));
	MALLOC_PROFILER(FMallocProfiler::SnapshotMemoryLoadMapStart(URL.Map));
	Error = TEXT("");

	FLoadTimeTracker::Get().ResetRawLoadTimes();

	FPlatformMisc::PreLoadMap(URL.Map, WorldContext.LastURL.Map, nullptr);

	// make sure level streaming isn't frozen
	if (WorldContext.World())
	{
		WorldContext.World()->bIsLevelStreamingFrozen = false;
	}

	// send a callback message
	FCoreUObjectDelegates::PreLoadMap.Broadcast(URL.Map);
	// make sure there is a matching PostLoadMap() no matter how we exit
	struct FPostLoadMapCaller
	{
		bool bCalled;
		FPostLoadMapCaller()
			: bCalled(false)
		{}
		~FPostLoadMapCaller()
		{
			if (!bCalled)
			{
				FCoreUObjectDelegates::PostLoadMap.Broadcast();
			}
		}
	} PostLoadMapCaller;

	// Cancel any pending texture streaming requests.  This avoids a significant delay on consoles 
	// when loading a map and there are a lot of outstanding texture streaming requests from the previous map.
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//UTexture2D::CancelPendingTextureStreaming();

	// play a load map movie if specified in ini
	bStartedLoadMapMovie = false;

	// clean up any per-map loaded packages for the map we are leaving
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//if (WorldContext.World() && WorldContext.World()->PersistentLevel)
	//{
	//	CleanupPackagesToFullyLoad(WorldContext, FULLYLOAD_Map, WorldContext.World()->PersistentLevel->GetOutermost()->GetName());
	//}

	// cleanup the existing per-game pacakges
	// @todo: It should be possible to not unload/load packages if we are going from/to the same GameMode.
	//        would have to save the game pathname here and pass it in to SetGameMode below
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//CleanupPackagesToFullyLoad(WorldContext, FULLYLOAD_Game_PreLoadClass, TEXT(""));
	//CleanupPackagesToFullyLoad(WorldContext, FULLYLOAD_Game_PostLoadClass, TEXT(""));
	//CleanupPackagesToFullyLoad(WorldContext, FULLYLOAD_Mutator, TEXT(""));


	// Cancel any pending async map changes after flushing async loading. We flush async loading before canceling the map change
	// to avoid completion after cancellation to not leave references to the "to be changed to" level around. Async loading is
	// implicitly flushed again later on during garbage collection.
	
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	FlushAsyncLoading();
	//CancelPendingMapChange(WorldContext);
	//WorldContext.SeamlessTravelHandler.CancelTravel();

	double	StartTime = FPlatformTime::Seconds();

	UE_LOG(LogLoad, Log, TEXT("LoadMap: %s"), *URL.ToString());
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	GInitRunaway();

	// Unload the current world
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	if (WorldContext.World())
	{
	//	// Display loading screen.
	//	if (!URL.HasOption(TEXT("quiet")))
	//	{
	//		TransitionType = TT_Loading;
	//		TransitionDescription = URL.Map;
	//		if (URL.HasOption(TEXT("Game=")))
	//		{
	//			TransitionGameMode = URL.GetOption(TEXT("Game="), TEXT(""));
	//		}
	//		else
	//		{
	//			TransitionGameMode = TEXT("");
	//		}
	//		LoadMapRedrawViewports();
	//		TransitionType = TT_None;
	//	}

		// Clean up networking
		//ShutdownWorldNetDriver(WorldContext.World());

		// Make sure there are no pending visibility requests.
		WorldContext.World()->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);

	//	// send a message that all levels are going away (NULL means every sublevel is being removed
	//	// without a call to RemoveFromWorld for each)
	//	//if (WorldContext.World()->GetNumLevels() > 1)
	//	{
	//		// TODO: Consider actually broadcasting for each level?
	//		FWorldDelegates::LevelRemovedFromWorld.Broadcast(nullptr, WorldContext.World());
	//	}

		// Disassociate the players from their PlayerControllers in this world.
		if (WorldContext.OwningGameInstance != nullptr)
		{
			for (auto It = WorldContext.OwningGameInstance->GetLocalPlayerIterator(); It; ++It)
			{
				ULocalPlayer *Player = *It;
				if (Player->PlayerController && Player->PlayerController->GetWorld() == WorldContext.World())
				{
					if (Player->PlayerController->GetPawn())
					{
						WorldContext.World()->DestroyActor(Player->PlayerController->GetPawn(), true);
					}
					WorldContext.World()->DestroyActor(Player->PlayerController, true);
					Player->PlayerController = nullptr;
				}
				// reset split join info so we'll send one after loading the new map if necessary
				Player->bSentSplitJoin = false;
			}
		}

		for (FActorIterator ActorIt(WorldContext.World()); ActorIt; ++ActorIt)
		{
			ActorIt->RouteEndPlay(EEndPlayReason::RemovedFromWorld);
		}

		// Do this after destroying pawns/playercontrollers, in case that spawns new things (e.g. dropped weapons)
		//WorldContext.World()->CleanupWorld();

	//	if (GEngine)
	//	{
	//		// clear any "DISPLAY" properties referencing level objects
	//		if (GEngine->GameViewport != nullptr)
	//		{
	//			ClearDebugDisplayProperties();
	//		}

	//		GEngine->WorldDestroyed(WorldContext.World());
	//	}
	//	WorldContext.World()->RemoveFromRoot();

	//	// mark everything else contained in the world to be deleted
	//	for (auto LevelIt(WorldContext.World()->GetLevelIterator()); LevelIt; ++LevelIt)
	//	{
	//		const ULevel* Level = *LevelIt;
	//		if (Level)
	//		{
	//			CastChecked<UWorld>(Level->GetOuter())->MarkObjectsPendingKill();
	//		}
	//	}

		// Stop all audio to remove references to current level.
		if (FAudioDevice* AudioDevice = WorldContext.World()->GetAudioDevice())
		{
			AudioDevice->Flush(WorldContext.World());
			AudioDevice->SetTransientMasterVolume(1.0f);
		}

	//	WorldContext.SetCurrentWorld(nullptr);
	}
//////////////////////////////////////////////////////////////////////////// ~ Unload the current world End
	// Clean up the previous level out of memory.
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, true);

	// For platforms which manage GPU memory directly we must Enqueue a flush, and wait for it to be processed
	// so that any pending frees that depend on the GPU will be processed.  Otherwise a whole map's worth of GPU memory
	// may be unavailable to load the next one.
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//ENQUEUE_UNIQUE_RENDER_COMMAND(FlushCommand,
	//{
	//	GRHICommandList.GetImmediateCommandList().ImmediateFlush(EImmediateFlushType::FlushRHIThreadFlushResources);
	//RHIFlushResources();
	//GRHICommandList.GetImmediateCommandList().ImmediateFlush(EImmediateFlushType::FlushRHIThreadFlushResources);
	//}
	//);
	//FlushRenderingCommands();

	//// Cancels the Forced StreamType for textures using a timer.
	//if (!IStreamingManager::HasShutdown())
	//{
	//	IStreamingManager::Get().CancelForcedResources();
	//}

	//if (FPlatformProperties::RequiresCookedData())
	//{
	//	appDefragmentTexturePool();
	//	appDumpTextureMemoryStats(TEXT(""));
	//}

//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//	// Dump info
//
//	Exec(NULL, TEXT("MEM"));
//
//	VerifyLoadMapWorldCleanup();
//
//#endif

	MALLOC_PROFILER(FMallocProfiler::SnapshotMemoryLoadMapMid(URL.Map); )

	WorldContext.OwningGameInstance->PreloadContentForURL(URL);

	//UPackage* WorldPackage = NULL;
	//UWorld*	NewWorld = NULL;

	// If this world is a PIE instance, we need to check if we are travelling to another PIE instance's world.
	// If we are, we need to set the PIERemapPrefix so that we load a copy of that world, instead of loading the
	// PIE world directly.
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//if (!WorldContext.PIEPrefix.IsEmpty())
	//{
	//	for (const FWorldContext& WorldContextFromList : WorldList)
	//	{
	//		// We want to ignore our own PIE instance so that we don't unnecessarily set the PIERemapPrefix if we are not travelling to
	//		// a server.
	//		if (WorldContextFromList.World() != WorldContext.World())
	//		{
	//			if (!WorldContextFromList.PIEPrefix.IsEmpty() && URL.Map.Contains(WorldContextFromList.PIEPrefix))
	//			{
	//				WorldContext.PIERemapPrefix = WorldContextFromList.PIEPrefix;
	//			}
	//		}
	//	}
	//}

	//// Is this a PIE networking thing?
	//if (!WorldContext.PIERemapPrefix.IsEmpty())
	//{
	//	if (URL.Map.Contains(WorldContext.PIERemapPrefix))
	//	{
	//		FString SourceWorldPackage = UWorld::RemovePIEPrefix(URL.Map);

	//		// We are loading a new world for this context, so clear out PIE fixups that might be lingering.
	//		// (note we dont want to do this in DuplicateWorldForPIE, since that is also called on streaming worlds.
	//		GPlayInEditorID = WorldContext.PIEInstance;
	//		FLazyObjectPtr::ResetPIEFixups();

	//		NewWorld = UWorld::DuplicateWorldForPIE(SourceWorldPackage, NULL);
	//		if (NewWorld == nullptr)
	//		{
	//			NewWorld = CreatePIEWorldByLoadingFromPackage(WorldContext, SourceWorldPackage, WorldPackage);
	//			if (NewWorld == nullptr)
	//			{
	//				Error = FString::Printf(TEXT("Failed to load package '%s' while in PIE"), *SourceWorldPackage);
	//				return false;
	//			}
	//		}
	//		else
	//		{
	//			WorldPackage = CastChecked<UPackage>(NewWorld->GetOuter());
	//		}

	//		NewWorld->StreamingLevelsPrefix = UWorld::BuildPIEPackagePrefix(WorldContext.PIEInstance);
	//		GIsPlayInEditorWorld = true;
	//	}
	//}

	const FString URLTrueMapName = URL.Map;
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//// Normal map loading
	//if (NewWorld == NULL)
	//{
	//	// Set the world type in the static map, so that UWorld::PostLoad can set the world type
	//	const FName URLMapFName = FName(*URL.Map);
	//	UWorld::WorldTypePreLoadMap.FindOrAdd(URLMapFName) = WorldContext.WorldType;

	//	// See if the level is already in memory
	//	WorldPackage = FindPackage(nullptr, *URL.Map);

	//	const bool bPackageAlreadyLoaded = (WorldPackage != nullptr);

	//	// If the level isn't already in memory, load level from disk
	//	if (WorldPackage == nullptr)
	//	{
	//		WorldPackage = LoadPackage(nullptr, *URL.Map, (WorldContext.WorldType == EWorldType::PIE ? LOAD_PackageForPIE : LOAD_None));
	//	}

	//	// Clean up the world type list now that PostLoad has occurred
	//	UWorld::WorldTypePreLoadMap.Remove(URLMapFName);

	//	if (WorldPackage == nullptr)
	//	{
	//		// it is now the responsibility of the caller to deal with a NULL return value and alert the user if necessary
	//		Error = FString::Printf(TEXT("Failed to load package '%s'"), *URL.Map);
	//		return false;
	//	}

	//	// Find the newly loaded world.
	//	NewWorld = UWorld::FindWorldInPackage(WorldPackage);

	//	// If the world was not found, it could be a redirector to a world. If so, follow it to the destination world.
	//	if (!NewWorld)
	//	{
	//		NewWorld = UWorld::FollowWorldRedirectorInPackage(WorldPackage);
	//		if (NewWorld)
	//		{
	//			WorldPackage = NewWorld->GetOutermost();
	//		}
	//	}
	//	check(NewWorld);

	//	NewWorld->PersistentLevel->HandleLegacyMapBuildData();

	//	FScopeCycleCounterUObject MapScope(WorldPackage);

	//	if (WorldContext.WorldType == EWorldType::PIE)
	//	{
	//		// If we are a PIE world and the world we just found is already initialized, then we're probably reloading the editor world and we
	//		// need to create a PIE world by duplication instead
	//		if (bPackageAlreadyLoaded)
	//		{
	//			if (WorldContext.PIEInstance == -1)
	//			{
	//				// Assume if we get here, that it's safe to just give a PIE instance so that we can duplicate the world 
	//				//	If we won't duplicate the world, we'll refer to the existing world (most likely the editor version, and it can be modified under our feet, which is bad)
	//				// So far, the only known way to get here is when we use the console "open" command while in a client PIE instance connected to non PIE server 
	//				// (i.e. multi process PIE where client is in current editor process, and dedicated server was launched as separate process)
	//				WorldContext.PIEInstance = 0;
	//			}

	//			NewWorld = CreatePIEWorldByDuplication(WorldContext, NewWorld, URL.Map);
	//			// CreatePIEWorldByDuplication clears GIsPlayInEditorWorld so set it again
	//			GIsPlayInEditorWorld = true;
	//		}
	//		// Otherwise we are probably loading new map while in PIE, so we need to rename world package and all streaming levels
	//		else if (Pending == NULL)
	//		{
	//			NewWorld->RenameToPIEWorld(WorldContext.PIEInstance);
	//		}
	//	}
	//	else if (WorldContext.WorldType == EWorldType::Game)
	//	{
	//		// If we are a game world and the world we just found is already initialized, then we're probably trying to load
	//		// an independent fresh copy of the world into a different context. Create a package with a prefixed name
	//		// and load the world from disk to keep the instances independent. If this is the case, assume the creator
	//		// of the FWorldContext was aware and set WorldContext.PIEInstance to a valid value.
	//		if (NewWorld->bIsWorldInitialized && WorldContext.PIEInstance != -1)
	//		{
	//			NewWorld = CreatePIEWorldByLoadingFromPackage(WorldContext, URL.Map, WorldPackage);

	//			if (NewWorld == nullptr)
	//			{
	//				Error = FString::Printf(TEXT("Failed to load package '%s' into a new game world."), *URL.Map);
	//				return false;
	//			}
	//		}
	//	}
	//}
	//NewWorld->SetGameInstance(WorldContext.OwningGameInstance);

	//GWorld = NewWorld;

	//WorldContext.SetCurrentWorld(NewWorld);
	//WorldContext.World()->WorldType = WorldContext.WorldType;

	//// Fixme: hacky but we need to set PackageFlags here if we are in a PIE Context.
	//// Also, don't add to root when in PIE, since PIE doesn't remove world from root
	//if (WorldContext.WorldType == EWorldType::PIE)
	//{
	//	check(WorldContext.World()->GetOutermost()->HasAnyPackageFlags(PKG_PlayInEditor));
	//	WorldContext.World()->ClearFlags(RF_Standalone);
	//}
	//else
	//{
	//	WorldContext.World()->AddToRoot();
	//}

	// In the PIE case the world will already have been initialized as part of CreatePIEWorldByDuplication
	if (!WorldContext.World()->bIsWorldInitialized)
	{
		WorldContext.World()->InitWorld();
	}

	// Handle pending level.
	if (Pending)
	{
		check(Pending == WorldContext.PendingNetGame);
		MovePendingLevel(WorldContext);
	}
	else
	{
		check(!WorldContext.World()->GetNetDriver());
	}

	WorldContext.World()->SetGameMode(URL);

	if (FAudioDevice* AudioDevice = WorldContext.World()->GetAudioDevice())
	{
		AudioDevice->SetDefaultBaseSoundMix(WorldContext.World()->GetWorldSettings()->DefaultBaseSoundMix);
	}

////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//// Listen for clients.
	//if (Pending == NULL && (!GIsClient || URL.HasOption(TEXT("Listen"))))
	//{
	//	if (!WorldContext.World()->Listen(URL))
	//	{
	//		UE_LOG(LogNet, Error, TEXT("LoadMap: failed to Listen(%s)"), *URL.ToString());
	//	}
	//}

	//const TCHAR* MutatorString = URL.GetOption(TEXT("Mutator="), TEXT(""));
	//if (MutatorString)
	//{
	//	TArray<FString> Mutators;
	//	FString(MutatorString).ParseIntoArray(Mutators, TEXT(","), true);

	//	for (int32 MutatorIndex = 0; MutatorIndex < Mutators.Num(); MutatorIndex++)
	//	{
	//		LoadPackagesFully(WorldContext.World(), FULLYLOAD_Mutator, Mutators[MutatorIndex]);
	//	}
	//}

	//// Process global shader results before we try to render anything
	//// Do this before we register components, as USkinnedMeshComponents require the GPU skin cache global shaders when creating render state.
	//if (GShaderCompilingManager)
	//{
	//	GShaderCompilingManager->ProcessAsyncResults(false, true);
	//}

	//{
	//	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UEngine::LoadMap.LoadPackagesFully"), STAT_LoadMap_LoadPackagesFully, STATGROUP_LoadTime);

	//	// load any per-map packages
	//	check(WorldContext.World()->PersistentLevel);
	//	LoadPackagesFully(WorldContext.World(), FULLYLOAD_Map, WorldContext.World()->PersistentLevel->GetOutermost()->GetName());

	//	// Make sure "always loaded" sub-levels are fully loaded
	//	WorldContext.World()->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);

	//	if (!GIsEditor && !IsRunningDedicatedServer())
	//	{
	//		// If requested, duplicate dynamic levels here after the source levels are created.
	//		WorldContext.World()->DuplicateRequestedLevels(FName(*URL.Map));
	//	}
	//}

	// Note that AI system will be created only if ai-system-creation conditions are met
	WorldContext.World()->CreateAISystem();

	// Initialize gameplay for the level.
	WorldContext.World()->InitializeActorsForPlay(URL);

	// calling it after InitializeActorsForPlay has been called to have all potential bounding boxed initialized
	UNavigationSystem::InitializeForWorld(WorldContext.World(), FNavigationSystemRunMode::GameMode);

	// Remember the URL. Put this before spawning player controllers so that
	// a player controller can get the map name during initialization and
	// have it be correct
	WorldContext.LastURL = URL;
	WorldContext.LastURL.Map = URLTrueMapName;

	if (WorldContext.World()->GetNetMode() == NM_Client)
	{
		WorldContext.LastRemoteURL = URL;
	}

	// Spawn play actors for all active local players
	if (WorldContext.OwningGameInstance != NULL)
	{
		for (auto It = WorldContext.OwningGameInstance->GetLocalPlayerIterator(); It; ++It)
		{
			FString Error2;
			if (!(*It)->SpawnPlayActor(URL.ToString(1), Error2, WorldContext.World()))
			{
				UE_LOG(LogEngine, Fatal, TEXT("Couldn't spawn player: %s"), *Error2);
			}
		}
	}

	// Prime texture streaming.
	//IStreamingManager::Get().NotifyLevelChange();
////// Cancel for simpleload /////////////////////////////////////////////////////////////////////////////////
	//if (GEngine && GEngine->HMDDevice.IsValid())
	//{
	//	GEngine->HMDDevice->OnBeginPlay(WorldContext);
	//}
	WorldContext.World()->BeginPlay();

	// send a callback message
	PostLoadMapCaller.bCalled = true;
	FCoreUObjectDelegates::PostLoadMap.Broadcast();

	WorldContext.World()->bWorldWasLoadedThisTick = true;

	// We want to update streaming immediately so that there's no tick prior to processing any levels that should be initially visible
	// that requires calculating the scene, so redraw everything now to take care of it all though don't present the frame.
	//RedrawViewports(false);

	// RedrawViewports() may have added a dummy playerstart location. Remove all views to start from fresh the next Tick().
	// IStreamingManager::Get().RemoveStreamingViews(RemoveStreamingViews_All);

	// See if we need to record network demos
	if (WorldContext.World()->GetAuthGameMode() == NULL || !WorldContext.World()->GetAuthGameMode()->IsHandlingReplays())
	{
		if (URL.HasOption(TEXT("DemoRec")) && WorldContext.OwningGameInstance != nullptr)
		{
			const TCHAR* DemoRecName = URL.GetOption(TEXT("DemoRec="), NULL);

			// Record the demo, optionally with the specified custom name.
			WorldContext.OwningGameInstance->StartRecordingReplay(FString(DemoRecName), WorldContext.World()->GetMapName(), URL.Op);
		}
	}

	STAT_ADD_CUSTOMMESSAGE_NAME(STAT_NamedMarker, *(FString(TEXT("LoadMapComplete - ") + URL.Map)));
	MALLOC_PROFILER(FMallocProfiler::SnapshotMemoryLoadMapEnd(URL.Map); )

	double StopTime = FPlatformTime::Seconds();

	UE_LOG(LogLoad, Log, TEXT("Took %f seconds to LoadMap(%s)"), StopTime - StartTime, *URL.Map);
	FLoadTimeTracker::Get().DumpRawLoadTimes();

	// Successfully started local level.
	return true;
}
