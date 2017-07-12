// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPlayerController.h"
#include "Player/PBPlayerCameraManager.h"
#include "Player/PBCheatManager.h"
#include "Player/PBLocalPlayer.h"
#include "Player/PBPlayerState.h"
#include "Weapons/PBWeapon.h"

#include "Online.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSessionInterface.h"
#include "PBGameInstance.h"
#include "PBGameViewportClient.h"
#include "PBWorldSettings.h"

#include "Blueprint/UserWidget.h"
#include "UMG/InGame/PBInGameWidget.h"
#include "UMG/InGame/PBInGameMenuWidget.h"
#include "UMG/Common/PBWidget_SystemMessageListBox.h"

#include "FPBWidgetEventDispatcher.h"
#include "Sound/SoundNodeLocalPlayer.h"

#include "Runtime/UMG/Public/Components/WidgetComponent.h"

#include "PBSceneCaptureComponent2D.h"

APBPlayerController::APBPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = APBPlayerCameraManager::StaticClass();
	CheatClass = UPBCheatManager::StaticClass();

	WidgetEventDispatcher = MakeShareable(new FPBWidgetEventDispatcher());
	bAutoManageActiveCameraTarget = false;

	InitInputManagement();
	InitInGameMenu();
	InitInGameWidget();
	InitSceneCaptureComp();
}

void APBPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StartUpCheatSys();

	StartUpSceneCaptureComp();
}

//InPlayer 에 UPBLocalPalyer 정보가 담겨져 있다.
void APBPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (IsLocalPlayerController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);

		StartUpInGameMenu();
	}

	if (Role == ROLE_Authority && IsLocalPlayerController())
	{
		SetPlayerStateInitiallyReplicated();
		const FPBUserBaseInfo& UserBaseInfo = GetPersistentUserBaseInfo();
		SetReplicateUserBaseInfo(UserBaseInfo);
	}
}

void APBPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	LoadSavedKeybindings();
}

void APBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	RegisterSituationBoardInputHandlers();
	RegisterInGameMenuInputHandlers();
}

void APBPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	SetInputAllowed(false);
	FadeBGM();
	UpdateSaveFileOnGameEnd(bIsWinner);

	NotifyMatchResultToInGameWidget(bIsWinner);
}

void APBPlayerController::PreClientTravel(const FString& PendingGameURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingGameURL, TravelType, bIsSeamlessTravel);

	//ShowLoadingScreenOnTravel();
}

// Once the PlayerState is replicated, the client is ready to start the game.
void APBPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalPlayerController())
	{
		SetPlayerStateInitiallyReplicated();
		const FPBUserBaseInfo& UserBaseInfo = GetPersistentUserBaseInfo();
		ServerUserReadyToStartMatch(UserBaseInfo);
	}
}

void APBPlayerController::ReceivedSpectatorClass(TSubclassOf<class ASpectatorPawn> SpectatorClass)
{
	if (IsInState(NAME_Spectating))
	{
		BeginSpectatingState();
	}
}

void APBPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void APBPlayerController::BeginDestroy()
{
	CleanUpInGameWidget();

	Super::BeginDestroy();
}

bool APBPlayerController::CanRestartPlayer()
{
	if (!Super::CanRestartPlayer())
	{
		return false;
	}

	if (!UserReadyToStart())
	{
		return false;
	}

	return true;
}

void APBPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);

	//if (GetWorld())
	//{
	//	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	//	{
	//		ActorList.Add(*It);
	//	}
	//}
}

void APBPlayerController::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{	
	// 게임 시작 대기중(WaitingToStart) 일 때 스폰할 위치로 카메라 위치,방향 설정(with 'bAutoManageActiveCameraTarget = false')
	// 또 다른 방법은 GetSpectatorPawn() 를 사용한다면 Spectator 클래스의 GetActorEyesViewPoint 에서 처리할 수도 있다.

	if (GetPawn() == nullptr)
	{
		OutResult.Location = GetSpawnLocation();
		OutResult.Location.Z += 64.f; // 캐릭터 눈높이 하드코딩
		OutResult.Rotation = GetSpawnRotation();		

		return;
	}
	
	OutResult.Location = GetFocalLocation();
	OutResult.Rotation = GetControlRotation();
}

void APBPlayerController::GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const
{
	// If we have a Pawn, this is our view point.
	if (GetPawnOrSpectator() != NULL)
	{		
		GetPawnOrSpectator()->GetActorEyesViewPoint(out_Location, out_Rotation);
		UE_LOG(LogStats, Display, TEXT("1 EyesViewPoint %s"), *FString::Printf(TEXT("cam Loc(%f: %f: %f) Rot(%f: %f: %f)"), out_Location.X, out_Location.Y, out_Location.Z, out_Rotation.Yaw, out_Rotation.Pitch, out_Rotation.Roll));
	}
	else
	{		
		out_Location = PlayerCameraManager ? PlayerCameraManager->GetCameraLocation() : GetSpawnLocation();
		out_Rotation = GetControlRotation();
		UE_LOG(LogStats, Display, TEXT("3 EyesViewPoint %s"), *FString::Printf(TEXT("cam Loc(%f: %f: %f) Rot(%f: %f: %f)"), out_Location.X, out_Location.Y, out_Location.Z, out_Rotation.Yaw, out_Rotation.Pitch, out_Rotation.Roll));
	}
}

void APBPlayerController::SetInitialLocationAndRotation(const FVector& NewLocation, const FRotator& NewRotation)
{
	Super::SetInitialLocationAndRotation(NewLocation, NewRotation);

	SetSpawnRotation(NewRotation);
}

void APBPlayerController::SetSpawnRotation(const FRotator& NewRotation)
{
	SpawnRotation = NewRotation;
	LastSpectatorSyncRotation = NewRotation;
}

bool APBPlayerController::ServerUserReadyToStartMatch_Validate(const FPBUserBaseInfo& MyUserBaseInfo)
{
	return true;
}
void APBPlayerController::ServerUserReadyToStartMatch_Implementation(const FPBUserBaseInfo& MyUserBaseInfo)
{
	SetPlayerStateInitiallyReplicated();
	SetReplicateUserBaseInfo(MyUserBaseInfo);

	APBGameMode* GM = GetWorld() ? Cast<APBGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
	if (GM)
	{
		GM->HandleStartingNewPlayer(this);
	}
}

void APBPlayerController::SetPlayerStateInitiallyReplicated()
{
	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		PBPlayerState->SetInitiallyReplicated();
	}
}

void APBPlayerController::SetReplicateUserBaseInfo(const FPBUserBaseInfo& MyUserBaseInfo)
{
	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		if (MyUserBaseInfo.EquippedCharItems.Num() > 0)
		{
			PBPlayerState->ReplicateEquippedCharItems = MyUserBaseInfo.EquippedCharItems;
		}
		if (MyUserBaseInfo.EquippedWeapItems.Num() > 0)
		{
			PBPlayerState->ReplicateEquippedWeapItems = MyUserBaseInfo.EquippedWeapItems;
		}
	}
}

void APBPlayerController::SetPlayerStarted()
{
	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		PBPlayerState->SetPlayerStarted();
	}
}

bool APBPlayerController::UserReadyToStart()
{
	auto PS = GetPBPlayerState();
	if (PS == nullptr)
	{
		return false;
	}

	if (!PS->IsInitiallyReplicated())
	{
		return false;
	}

	if (!PS->HasCharacterItems() || !PS->HasWeaponItems())
	{
		return false;
	}

	return true;
}

bool APBPlayerController::UserStarted()
{
	auto PS = GetPBPlayerState();
	if (PS == nullptr)
	{
		return false;
	}

	if (!PS->IsPlayerStarted())
	{
		return false;
	}

	return true;
}

void APBPlayerController::ShowLoadingScreenOnTravel()
{
	if (GetWorld() != NULL)
	{
		UPBGameViewportClient* PBViewport = Cast<UPBGameViewportClient>(GetWorld()->GetGameViewport());

		if (PBViewport != NULL)
		{
			PBViewport->ShowLoadingScreen();
		}
	}
}

void APBPlayerController::ClientReturnToMainMenu_Implementation(const FString& InReturnReason)
{
	// This is only notified from server to remote clients
	ensure(GetNetMode() == NM_Client);

	UPBGameInstance* GI = GetWorld() != NULL ? Cast<UPBGameInstance>(GetWorld()->GetGameInstance()) : NULL;
	if (GI)
	{
		if (!InReturnReason.IsEmpty())
		{
			const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
			GI->ShowMessageThenGotoState(FText::FromString(InReturnReason), OKButton, FText::GetEmpty(), PBGameInstanceState::MainMenu);
		}
		else
		{
			GI->GotoState(PBGameInstanceState::MainMenu);
		}
	}
}

void APBPlayerController::ClientGameStarted_Implementation()
{
	SetInputAllowed(true);
	PlayBGM();
	GrantUnbeatability();

	SetInGameWidgetState(MatchState::InProgress);
	WidgetEventDispatcher->OnMissionRenewed.Broadcast();
}

void APBPlayerController::ClientWarmupStarted_Implementation()
{
	SetInGameWidgetState(MatchState::WaitingToStart);

	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnWarmupStarted.Broadcast();
	}
}

void APBPlayerController::ClientWarmupEnded_Implementation()
{
}

void APBPlayerController::ClientConcludeMatchStarted_Implementation()
{
	SetInGameWidgetState(MatchState::WaitingPostMatch);



}

void APBPlayerController::ClientConcludeMatchEnded_Implementation()
{
}

void APBPlayerController::ClientConcludeAllMatchesStarted_Implementation()
{
	SetInGameWidgetState(MatchState::WaitingPostMatch, (uint8)EPBPostMatchState::ConcludeAllMatches);
}

void APBPlayerController::ClientConcludeAllMatchesEnded_Implementation()
{
}

void APBPlayerController::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	NotifyWidgetOfDeath(KillerPlayerState, KilledPlayerState, KillerDamageType, TakeDamageInfo);
}

void APBPlayerController::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnSpawn.Broadcast(SpawnedPlayerState);
	}
}

void APBPlayerController::ShowInGameMenu()
{
	if (InGameMenuWidget && !InGameMenuWidget->IsOpened())
	{
		InGameMenuWidget->ToggleGameMenu();
	}
}

void APBPlayerController::OnToggleInGameMenu()
{
	if (GEngine->GameViewport == nullptr)
	{
		return;
	}

	UWorld* GameWorld = GEngine->GameViewport->GetWorld();
	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* Controller = Cast<APBPlayerController>(*It);
		if (Controller && Controller->IsPaused())
		{
			return;
		}
	}

	if (InGameMenuWidget)
	{
		InGameMenuWidget->ToggleGameMenu();
	}
}

bool APBPlayerController::IsGameMenuVisible() const
{
	bool Result = false;

	if (InGameMenuWidget)
		Result = InGameMenuWidget->IsOpened();

	return Result;
}

void APBPlayerController::InitInGameMenu()
{
	InGameMenuWidget = nullptr;
}

void APBPlayerController::StartUpInGameMenu()
{
	if (InGameMenuWidget == nullptr && InGameMenuClass)
	{
		InGameMenuWidget = CreateWidget<UPBInGameMenuWidget>(this, InGameMenuClass);
	}
}

void APBPlayerController::RegisterInGameMenuInputHandlers()
{
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &APBPlayerController::OnToggleInGameMenu);
}

void APBPlayerController::InitSceneCaptureComp()
{
	if (IsLocalPlayerController())
	{
		PBSceneCaptureComp2D = CreateDefaultSubobject<UPBSceneCaptureComponent2D>("PPSceneCaptureComp2D");
		if (PBSceneCaptureComp2D)
		{
			PBSceneCaptureComp2D->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

}

void APBPlayerController::StartUpSceneCaptureComp()
{
	if (PBSceneCaptureComp2D)
	{
		if (IsLocalPlayerController() && RenderTargetForBlur)
		{
			PBSceneCaptureComp2D->TextureTarget = RenderTargetForBlur;
			PBSceneCaptureComp2D->Activate();
		}
		else
		{
			PBSceneCaptureComp2D->TextureTarget = nullptr;
			PBSceneCaptureComp2D->Deactivate();
		}

	}


}

bool APBPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

void APBPlayerController::InitInputManagement()
{
	bAllowGameActions = true;
	SetIgnoreMoveInput(false);
}

void APBPlayerController::SetInputAllowed(bool Value)
{
	if (bAllowGameActions && !Value)
	{
		// We are disabling input - ensure the chracter is in resting state
		APBCharacter* C = Cast<APBCharacter>(GetCharacter());
		if (C && C->IsLocallyControlled())
		{
			if (C->IsJumpProvidingForce())
			{
				C->OnStopJump();
			}
			if (C->IsSprinting())
			{
				C->OnStopSprinting();
			}
			if (C->IsWalking())
			{
				C->OnStopWalking();
			}
			if (C->IsZooming())
			{
				C->OnZoomOut();
			}
			if (C->IsFiring())
			{
				C->OnEndFireTrigger1();
			}
			if (C->IsPlantingBomb())
			{
				C->OnEndPlantBomb();
			}
			if (C->IsDefusingBomb())
			{
				C->OnEndDefuseBomb();
			}
		}
	}

	bAllowGameActions = Value;
	SetIgnoreMoveInput(!Value);
}

void APBPlayerController::GrantUnbeatability()
{
	if (GetWorld())
	{
		APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
		APBCharacter* C = Cast<APBCharacter>(GetPawn());
		if (C)
		{
			C->UnbeatableOn(GM->UnbeatableTimeUponSpawn);
		}
	}
}

void APBPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	APBPlayerState* PS = Cast<APBPlayerState>(PlayerState);
	if (PS)
	{
		PS->OwnerCharacter = Cast<APBCharacter>(aPawn);
		//bFindCameraComponentWhenViewTarget = true;
	}
}

void APBPlayerController::UnPossess()
{
	APBPlayerState* PS = Cast<APBPlayerState>(PlayerState);
	if (PS)
	{
		PS->OwnerCharacter = nullptr;
	}

	Super::UnPossess();
}

void APBPlayerController::RestartPlayerCharacter()
{
	if (Role == ROLE_Authority)
	{
		if (GetWorld())
		{
			APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				if (GM->PlayerCanRestart(this))
				{
					if (GetPawn())
					{
						GetPawn()->SetLifeSpan(0.1f);
					}

					UnPossess();

					// Respawn
					GM->RestartPlayer(this);

					// Unbeatable
					GrantUnbeatability();
				}
			}
		}
	}
}

void APBPlayerController::ClientSetInGameWidget_Implementation(TSubclassOf<class UPBInGameWidget> NewInGameWidgetCLass)
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (NewInGameWidgetCLass)
	{
		InGameWidget = CreateWidget<UPBInGameWidget>(this, NewInGameWidgetCLass);
		if (InGameWidget)
		{
			InGameWidget->Show();
		}
	}
}

UPBInGameWidget* APBPlayerController::GetInGameWidget()
{
	return InGameWidget;
}

void APBPlayerController::InitInGameWidget()
{
	InGameWidget = nullptr;
}

void APBPlayerController::SetInGameWidgetState(FName MatchState, uint8 MatchSubstateIndex /*= 0*/)
{
	if (InGameWidget)
	{
		InGameWidget->SetState(MatchState, MatchSubstateIndex);
	}
}

void APBPlayerController::NotifyMatchResultToInGameWidget(bool bIsWinner)
{
	if (InGameWidget)
	{
		InGameWidget->NotifyMatchResult(bIsWinner);
	}
}

void APBPlayerController::CleanUpInGameWidget()
{
	if (InGameWidget)
	{
		InGameWidget->Hide();
		InGameWidget = nullptr;
	}
}

void APBPlayerController::OnToggleScoreboard()
{
	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnToggleSituation.Broadcast();
	}
}

void APBPlayerController::OnShowScoreboard()
{
	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnShowSituation.Broadcast(true);
	}
}

void APBPlayerController::OnHideScoreboard()
{
	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnShowSituation.Broadcast(false);
	}
}

void APBPlayerController::RegisterSituationBoardInputHandlers()
{
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &APBPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &APBPlayerController::OnHideScoreboard);
	InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &APBPlayerController::OnToggleScoreboard);
}

void APBPlayerController::NotifyWidgetOfDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (WidgetEventDispatcher.IsValid())
	{
		WidgetEventDispatcher->OnDeath.Broadcast(KillerPlayerState, KilledPlayerState, KillerDamageType, TakeDamageInfo);
	}
}

void APBPlayerController::PlayBGM()
{
	APBWorldSettings* WS = Cast<APBWorldSettings>(GetWorldSettings());
	if (WS)
	{
		WS->PlayBGM();
	}
}

void APBPlayerController::FadeBGM()
{
	APBWorldSettings* WS = Cast<APBWorldSettings>(GetWorldSettings());
	if (WS)
	{
		WS->FadeOutBGM();
	}
}

APBPlayerState* APBPlayerController::GetPBPlayerState()
{
	return PlayerState ? Cast<APBPlayerState>(PlayerState) : nullptr;
}

UPBPersistentUser* APBPlayerController::GetPersistentUser() const
{
	UPBLocalPlayer* const PBLocalPlayer = Cast<UPBLocalPlayer>(Player);
	return PBLocalPlayer ? PBLocalPlayer->GetPersistentUser() : nullptr;
}

FPBUserBaseInfo& APBPlayerController::GetPersistentUserBaseInfo()
{
	if (GetPersistentUser())
	{
		return GetPersistentUser()->GetBaseUserInfo();
	}

	static FPBUserBaseInfo EmptyUserInfo;
	return EmptyUserInfo;
}

void APBPlayerController::LoadSavedKeybindings()
{
	UPBPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
}

void APBPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{
	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		// update local saved profile
		UPBPersistentUser* const PersistentUser = GetPersistentUser();
		if (PersistentUser)
		{
			PersistentUser->AddMatchResult(PBPlayerState->GetKills(), PBPlayerState->GetDeaths(), bIsWinner);
			PersistentUser->SaveIfDirty();
		}
	}
}

void APBPlayerController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *GameSessionName.ToString());
				Sessions->StartSession(GameSessionName);
			}
		}
	}
	else
	{
		// Keep retrying until player state is replicated
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &APBPlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
		}		
	}
}

void APBPlayerController::ClientEndOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	APBPlayerState* PBPlayerState = Cast<APBPlayerState>(PlayerState);
	if (PBPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *GameSessionName.ToString());
				Sessions->EndSession(GameSessionName);
			}
		}
	}
}

bool APBPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void APBPlayerController::ServerCheat_Implementation(const FString& Msg)
{
	if (CheatManager)
	{
		ClientMessage(ConsoleCommand(Msg));
	}
}

void APBPlayerController::SetInfiniteAmmo(bool bEnable)
{
	APBCharacter* MyPawn = Cast<APBCharacter>(GetPawn());
	APBWeapon* MyWeapon = MyPawn ? MyPawn->GetCurrentWeapon() : NULL;
	if (MyWeapon)
	{
		MyWeapon->FillAmmo();
	}

	bInfiniteAmmo = bEnable;
}

bool APBPlayerController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

void APBPlayerController::SetInfiniteClip(bool bEnable)
{
	APBCharacter* MyPawn = Cast<APBCharacter>(GetPawn());
	APBWeapon* MyWeapon = MyPawn ? MyPawn->GetCurrentWeapon() : NULL;
	if (MyWeapon)
	{
		MyWeapon->FillAmmo();
	}

	bInfiniteClip = bEnable;
}

bool APBPlayerController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

void APBPlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

bool APBPlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

void APBPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

bool APBPlayerController::HasGodMode() const
{
	return bGodMode;
}

void APBPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		const float CurrentTime = GetWorld() ? GetWorld()->TimeSeconds : 0.f;
		if ((GetPawn() != NULL) && ((CurrentTime - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
		{
			APBCharacter* MyPawn = Cast<APBCharacter>(GetPawn());
			if (MyPawn)
			{
				MyPawn->Suicide();
			}
		}
	}
}

void APBPlayerController::StartUpCheatSys()
{
	// The engine enable cheat only on host by default. Now we enable cheat for all.
	EnableCheats();
}

void APBPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APBPlayerController, bInfiniteAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBPlayerController, bInfiniteClip, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBPlayerController, bHealthRegen, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBPlayerController, bGodMode, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBPlayerController, SpawnRotation, COND_OwnerOnly);
}