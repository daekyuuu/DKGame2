// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBGameInstance.h"
#include "Player/PBSpectatorPawn.h"
#include "Player/PBPlayerState.h"
#include "Online/PBUserInfo.h"
#include "PBGameMode.h"
#include "Online/PBGameSession.h"
#include "Bots/PBAIController.h"
#include "Mode/SpawnPoint/PBPlayerStart.h"
#include "LevelSequenceActor.h"
#include "Table/Game/PBGameModeTable.h"
#include "Table/Game/PBGameTableManager.h"
#include "Table/Item/PBItemTableManager.h"
#include "Table/Item/PBItemTable_Char.h"
#include "PBMission.h"
#include "PBProjectile.h"
#include "Pickups/PBPickup.h"
#include "Mode/PBMode_BombMission.h"
#include "PBPickupWeapon.h"

APBGameMode::APBGameMode()
{
	bUseSeamlessTravel = true;

	GameStateClass = APBGameState::StaticClass();
	PlayerControllerClass = APBPlayerController::StaticClass();
	PlayerStateClass = APBPlayerState::StaticClass();
	DefaultPawnClass = APBCharacter::StaticClass();
	SpectatorClass = APBSpectatorPawn::StaticClass();

	MinRespawnDelay = 5.0f;

	InitMatchCycle();
	InitWarmUpBroadcastSys();
	InitPlayerScoreSys();
	InitTeamSys();
	InitMissionSys();
	InitSpawnSys();
	InitDamageModSys();
	InitTableSys();
	InitPickup();



}

TSubclassOf<AGameSession> APBGameMode::GetGameSessionClass() const
{
	return APBGameSession::StaticClass();
}

void APBGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	FString ModeShortName = UGameplayStatics::ParseOption(Options, TEXT("game"));

	BotSysHandleInitGame(Options);
	TeamScoreHandleInitGame(Options);

	Super::InitGame(MapName, Options, ErrorMessage);
	DeterminePausability();
}

void APBGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void APBGameMode::BeginPlay()
{
	Super::BeginPlay();
	TeamScoreHandleBeginPlay();

	// Notifies game instance we are ready to receive players
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->OnGameReadyEvt.Broadcast();
	}
}

void APBGameMode::BeginDestroy()
{
	for (auto Elem : PickupItemMap)
	{
		if (Elem.Value)
		{
			Elem.Value->Destroy();
		}
	}

	Super::BeginDestroy();
}

void APBGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	DecideMissionsBeforeWarmUp();
	BotSysHandleWarmUp();

	UpdateMatchCycleOnWaitingToStart();
}

void APBGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	ReportMatchStarted();
	BotSysHandleMatchStart();

	UpdateMatchCycleOnStarted();
}

void APBGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	ConcludeMissionAfterMatch();

	UpdateMatchCycleOnEnded();
}

void APBGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	const bool bMissedJoinChance = !(GetMatchState() == MatchState::WaitingToStart || GetMatchState() == MatchState::InProgress);
	if (bMissedJoinChance)
	{
		ErrorMessage = TEXT("Missed chance to join room!");
	}
	else
	{
		// GameSession can be NULL if the match is over
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
}

// Called On Login
FString APBGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	ChooseTeamUponLogin(NewPlayerController, Options);
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void APBGameMode::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
	NewPlayer->ClientSetLocation(StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
}

// Match start to player
void APBGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 최초 게임모드 진입시
	// 1. Called RestartPlayer within PostLogin - HandleStartingNewPlayer(NewPlayer)
	// 2. 하지만 아직 플레이어는 PlayerState 가 replecate 되지 않은 상태이므로 이 단계에서 Match Start 되지 않는다.
	// 3. 클라이언트에서 PlayerState 가 리플리케이트 되면 서버에 시작하겠다고 RPC 로 요청하고 (APBPlayerController::ServerUserReadyToStartMatch)
	//    비로소 서버는 플레이어에게 Match Start 를 수행한다. (HandleStartingNewPlayer 함수 콜)

	// 게임모드의 라운드 재시작시
	// 1. Called RestartPlayer within PostLogin - HandleStartingNewPlayer(NewPlayer)
	// 2, 3 번은 수행되지 않는다 (이미 PlayerState 가 replicate 되었으므로)

	Super::PostLogin(NewPlayer);

	HandleWarmUpBroadcastOnLogin(NewPlayer);
	HandleMatchStartBroadcastOnLogin(NewPlayer);

	UpdateMatchCycleOnPlayerLogIn();
}

// Called in Super::PostLogin(NewPlayer);
void APBGameMode::InitializeHUDForPlayer_Implementation(APlayerController* NewPlayer)
{
	ReportInGameWidgetClasses(NewPlayer); 
}

bool APBGameMode::HaveToBeSpectator()
{
	auto GS = GetGameState<APBGameState>();
	if (false == ensureMsgf(nullptr != GS, TEXT("it has to need a valid instance of GameState.")))
		return false;

	for (FConstPlayerControllerIterator Itr = GetWorld()->GetPlayerControllerIterator(); Itr; ++Itr)
	{
		auto CurPC = Cast<APBPlayerController>(*Itr);
		auto CurPS = Cast<APBPlayerState>(CurPC->PlayerState);
		if (CurPC && CurPC->IsLocalPlayerController() && CurPS)
		{
			//@todo: WaittingRoom에서 다른 플레이어들과 함께 Ready상태에서 진입했는지의 여부를 알아내어야한다.
		}
	}
	//!+//@note: 현재 다음 조건으로 게임이 시작할 때 Player를 Spectator로 만들게 됩니다.
	//!+여기서 정의된 Spectator는 APlayerState에 포함된 bIsSpectator와 다른 것입니다.
	
	//1. Round 중 부활이 없는 경우 기본적으로 관전상태로 다음 Round까지 기다려야 한다.
	bool bHasRespawnableRule = Cast<APBMode_BombMission>(this) ? false : true;
	//bool bLessThan90Percent = ((float)(RoundTime) * 0.9f) > (float)(GS->RemainingTime);
	
	//2. 하지만 Round시 시작 후 시간이 30%를 초과하지 않았다면 즉시 시작할 수 있다.
	bool bLessThan30Percent = ((float)(RoundTime) * 0.3f) > (float)(GS->RemainingTime);

	return /*(false == bHasRespawnableRule && false == bLessThan90Percent) || */ bLessThan30Percent;
}

void APBGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	auto PC = Cast<APBPlayerController>(NewPlayer);
	if (PC)
	{
		if (!PC->CanRestartPlayer())
			return;

		if (PC->UserStarted())
			return;

		Super::HandleStartingNewPlayer_Implementation(NewPlayer);

		{
			//@todo: Pawn이 존재하며, Pawn을 비활성화 한 상태에서 Spectator로써의 역할을 하도록 한다.
			// 이 때 발생할 수 있는 상황들을 사전에 리스팅해본다.
			// 1. 현재 죽을 때 Spectator가 되는 조건은 Health가 0일 경우이다. 어떻게 처리할까?
			// 2. 1번을 해결했다면 APBCharacter::SpiritLeaveBody()를 사용 할 경우 문제는 없을까?
			if (HaveToBeSpectator())
			{
				auto C = Cast<APBCharacter>(PC->GetCharacter());
				if (C)
				{
					//C->ServerSetSpectator(true);
					C->ServerBecomeSpectatorOnAlive();
				}
			}
		}

		PC->SetPlayerStarted();
	}
}

void APBGameMode::HandleReturnToMainMenu(bool UnexpectedFromClient)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APBPlayerController* Controller = Cast<APBPlayerController>(*Iterator);
		if (Controller && !Controller->IsLocalController())
		{
			if (UnexpectedFromClient)
			{
				const FString RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.").ToString();
				Controller->ClientReturnToMainMenu(RemoteReturnReason);
			}
			else
			{
				Controller->ClientReturnToMainMenu(FString());
			}
		}
	}

	SetMatchState(MatchState::WaitingToStart);

	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		GS->SetMatchState(MatchState::WaitingToStart);
	}
}

void APBGameMode::DeterminePausability()
{
	UPBGameInstance* GameInstance = Cast<UPBGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		bPauseable = !GameInstance->GetIsOnline();
	}
}

bool APBGameMode::ReadyToStartMatch_Implementation()
{
	return false;
}

bool APBGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}

void APBGameMode::RestartGame()
{
	if (GetWorld())
	{
		for (auto Controller = GetWorld()->GetControllerIterator(); Controller; ++Controller)
		{
			if (Controller->IsValid())
			{
				Controller->Get()->UnPossess();
			}
		}

		for (TObjectIterator<AActor> Itr; Itr; ++Itr)
		{
			if (Cast<APBProjectile>(*Itr) || Cast<APBCharacter>(*Itr))
			{
				// Clean up every projectile and character
				(*Itr)->Destroy();
			}
			else if (Itr && !Cast<APlayerController>(*Itr))
			{
				// Reset non-controller actors
				Itr->Reset();
			}
		}

		SetMatchState(MatchState::WaitingToStart);
		APBGameState* const GS = Cast<APBGameState>(GameState);
		if (GS)
		{
			GS->SetMatchState(MatchState::WaitingToStart);
		}
	}

	PostRestartGameEvt.Broadcast();
}

void APBGameMode::PreConcludeMatch()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientConcludeMatchStarted();
		}
	}
}

void APBGameMode::PostConcludeMatch()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientConcludeMatchEnded();
		}
	}
}

bool APBGameMode::MatchIsFinalMatch()
{
	return false;
}

void APBGameMode::PreConcludeAllMatches()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientConcludeAllMatchesStarted();
		}
	}
}

void APBGameMode::PostConcludeAllMatches()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientConcludeAllMatchesEnded();
		}
	}
}

void APBGameMode::InitMatchCycle()
{
	WarmupTime = 3;
	RoundTime = 300;
	TimeToConcludeMatch = 15;
	TimeToConcludeAllMatches = 0;

	PreMatchState = EPBPreMatchState::None;
	PostMatchState = EPBPostMatchState::None;
}

float APBGameMode::GetTargetWarmupTime()
{
	bool FoundIntroSequence = false;
	float MaxIntroSequenceLength = 0.f;
	for (TActorIterator<ALevelSequenceActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ALevelSequenceActor* Seq = *ActorItr;
		if (Seq && Seq->SequencePlayer && Seq->ActorHasTag("Intro"))
		{
			FoundIntroSequence = true;
			MaxIntroSequenceLength = FMath::Max(MaxIntroSequenceLength, Seq->SequencePlayer->GetLength() * Seq->SequencePlayer->GetPlayRate());
		}
	}

	return FoundIntroSequence ? FMath::CeilToInt(MaxIntroSequenceLength) : WarmupTime;
}

void APBGameMode::UpdateMatchCycleOnWaitingToStart()
{
	// Enter sub cycle
	ScheduleProgressMatchCycle(0);
}

void APBGameMode::UpdateMatchCycleOnPlayerLogIn()
{
	// Maybe resume sub cycle
	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			if (PreMatchState == EPBPreMatchState::Idle)
			{
				if (GS->PlayerArray.Num() > 0 && !bDelayedStart)
				{
					WarmUpStartedEvt.Broadcast();
					PreMatchState = EPBPreMatchState::WarmUp;
					ScheduleProgressMatchCycle(GetTargetWarmupTime());
				}
			}
		}
	}
}

void APBGameMode::UpdateMatchCycleOnStarted()
{
	APBGameState* GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		GS->ServerIncreaseRound();
	}

	ScheduleProgressMatchCycle(RoundTime);
}

void APBGameMode::UpdateMatchCycleOnEnded()
{
	// Enter sub cycle
	ScheduleProgressMatchCycle(0);
}

void APBGameMode::PauseProgressMatchCycle()
{
	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (!GS) return;

	GetWorldTimerManager().ClearTimer(Handle_CountdownToProgressMatchCycle);
	GS->RemainingTime = 0;
}

void APBGameMode::ScheduleProgressMatchCycle(int32 DelaySecs)
{
	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (!GS) return;

	if (DelaySecs == 0)
	{
		// We cannot progress match cycle immediately here to avoid recursive call to SetmatchState.
		GetWorldTimerManager().ClearTimer(Handle_CountdownToProgressMatchCycle);
		GS->RemainingTime = DelaySecs;
		GetWorldTimerManager().SetTimer(Handle_CountdownToProgressMatchCycle, this, &APBGameMode::CountdownToProgressMatchCycle, 0.01, true);
	}
	else
	{		
		GetWorldTimerManager().ClearTimer(Handle_CountdownToProgressMatchCycle);
		GS->RemainingTime = DelaySecs;
		GetWorldTimerManager().SetTimer(Handle_CountdownToProgressMatchCycle, this, &APBGameMode::CountdownToProgressMatchCycle, GetWorldSettings()->GetEffectiveTimeDilation(), true);
	}
}

void APBGameMode::CountdownToProgressMatchCycle()
{
	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (GS && !GS->bTimerPaused)
	{
		if (GS->RemainingTime > 0)
		{
			GS->RemainingTime--;
		}

		if (GS->RemainingTime == 0)
		{
			ProgressMatchCycle();
		}
	}
}

void APBGameMode::ProgressMatchCycle()
{
	APBGameState* const GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			if (PreMatchState == EPBPreMatchState::None)
			{
				if (GS->PlayerArray.Num() > 0 && !bDelayedStart)
				{
					WarmUpStartedEvt.Broadcast();
					PreMatchState = EPBPreMatchState::WarmUp;
					ScheduleProgressMatchCycle(GetTargetWarmupTime());
				}
				else
				{
					// Wait until a player is logged in and try again
					PreMatchState = EPBPreMatchState::Idle;
					PauseProgressMatchCycle();
				}
			}
			else if (PreMatchState == EPBPreMatchState::Idle)
			{
				// Handled directly in UpdateMatchCycleOnPlayerLogIn()
			}
			else if (PreMatchState == EPBPreMatchState::WarmUp)
			{
				WarmUpEndedEvt.Broadcast();
				PreMatchState = EPBPreMatchState::None;
				StartMatch();
			}
		}
		else if (GetMatchState() == MatchState::InProgress)
		{
			MatchTimeUpEvt.Broadcast();
			EndMatch();
		}
		else if (GetMatchState() == MatchState::WaitingPostMatch)
		{
			if (PostMatchState == EPBPostMatchState::None)
			{
				PreConcludeMatch();
				PostMatchState = EPBPostMatchState::ConcludeMatch;
				ScheduleProgressMatchCycle(TimeToConcludeMatch);
			}
			else if (PostMatchState == EPBPostMatchState::ConcludeMatch)
			{
				PostConcludeMatch();

				if (MatchIsFinalMatch())
				{
					PreConcludeAllMatches();
					PostMatchState = EPBPostMatchState::ConcludeAllMatches;
					ScheduleProgressMatchCycle(TimeToConcludeAllMatches);
				}
				else
				{
					PostMatchState = EPBPostMatchState::None;
					RestartGame();
				}
			}
			else if (PostMatchState == EPBPostMatchState::ConcludeAllMatches)
			{
				PostConcludeAllMatches();

				UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance());
				if (GI)
				{
					PostMatchState = EPBPostMatchState::None;
					if (!IsRunningDedicatedServer())
					{
						// Host go back to main menu
						// This will also ask everyone to go back to main menu
						GI->ReturnToMainMenu(false);
					}
					else
					{
						// Only ask everyone to go back to main menu
						HandleReturnToMainMenu(false);

						// We are now at MatchState::WaitingPostMatch---PostMatchState::None
						// Do nothing and wait until we are asked by dedicated server manager to load a new map
					}
				}
			}
		}
	}
}

void APBGameMode::InitWarmUpBroadcastSys()
{
	WarmUpStartedEvt.AddUObject(this, &APBGameMode::ReportWarmUpStarted);
	WarmUpEndedEvt.AddUObject(this, &APBGameMode::ReportWarmUpEnded);
}

void APBGameMode::ReportWarmUpStarted()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientWarmupStarted();
		}
	}
}

void APBGameMode::ReportWarmUpEnded()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientWarmupEnded();
		}
	}
}

void APBGameMode::HandleWarmUpBroadcastOnLogin(APlayerController* NewPlayer)
{
	APBPlayerController* NewPC = Cast<APBPlayerController>(NewPlayer);
	if (NewPC)
	{
		if (GetMatchState() == MatchState::WaitingToStart && PreMatchState == EPBPreMatchState::WarmUp)
		{
			NewPC->ClientWarmupStarted();
		}
	}
}

void APBGameMode::ReportMatchStarted()
{
	if (GetWorld() == nullptr)
		return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

void APBGameMode::HandleMatchStartBroadcastOnLogin(APlayerController* NewPlayer)
{
	APBPlayerController* NewPC = Cast<APBPlayerController>(NewPlayer);
	if (NewPC)
	{
		if (GetMatchState() == MatchState::InProgress)
		{
			NewPC->ClientGameStarted();
			NewPC->ClientStartOnlineGame();
		}
	}
}

void APBGameMode::Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType, AActor* DamageCauser, const FTakeDamageInfo& TakeDamageInfo)
{
	APBPlayerState* KillerPlayerState = Killer ? Cast<APBPlayerState>(Killer->PlayerState) : nullptr;
	APBPlayerState* VictimPlayerState = KilledPlayer ? Cast<APBPlayerState>(KilledPlayer->PlayerState) : nullptr;

	OnKilledEvt.Broadcast(Killer, KilledPlayer);
	BroadcastDeath(KillerPlayerState, VictimPlayerState, DamageType, TakeDamageInfo);
}

void APBGameMode::BroadcastDeath(APBPlayerState* KillerPlayerState, APBPlayerState* VictimPlayerState, const UDamageType* DamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (VictimPlayerState)
	{
		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState, TakeDamageInfo);
	}
}

void APBGameMode::BroadcastSpawn(class AController* NewPlayer)
{
	if (NewPlayer)
	{
		APBPlayerState* SpawnedPS = Cast<APBPlayerState>(NewPlayer->PlayerState);
		if (SpawnedPS)
		{
			SpawnedPS->BroadcastSpawn();
		}
	}	
}

void APBGameMode::InitPlayerScoreSys()
{
	PlayerKillScore = 0;
	PlayerDeathScore = 0;

	OnKilledEvt.AddUObject(this, &APBGameMode::UpdatePlayerScores);
}

void APBGameMode::UpdatePlayerScores(AController* Killer, AController* KilledPlayer)
{
	APBPlayerState* KillerPlayerState = Killer ? Cast<APBPlayerState>(Killer->PlayerState) : nullptr;
	APBPlayerState* VictimPlayerState = KilledPlayer ? Cast<APBPlayerState>(KilledPlayer->PlayerState) : nullptr;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, PlayerKillScore);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, PlayerDeathScore);
	}
}

FString APBGameMode::GetTeamOptionName()
{
	return FString(TEXT("Team"));
}

EPBTeamType APBGameMode::ChooseTeam(class APBPlayerState* ForPlayerState) const
{
	return EPBTeamType::Alpha;
}

void APBGameMode::InitTeamSys()
{
}

void APBGameMode::ChooseTeamUponLogin(APlayerController* NewPlayer, const FString& Options)
{
	APBPlayerState* NewPlayerState = CastChecked<APBPlayerState>(NewPlayer->PlayerState);

	EPBTeamType TeamOptionValue = (EPBTeamType)UPBGameplayStatics::GetIntOption(Options, GetTeamOptionName(), (int32)EPBTeamType::Max);
	if (TeamOptionValue != EPBTeamType::Max)
	{
		NewPlayerState->SetTeamType(TeamOptionValue);
	}
	else
	{
		NewPlayerState->SetTeamType(ChooseTeam(NewPlayerState));
	}
}

int32 APBGameMode::GetTargetTeamScore(const FString& Options)
{
	return MAX_int32;
}

void APBGameMode::TeamScoreHandleInitGame(const FString& Options)
{
	TargetScoreCache = GetTargetTeamScore(Options);
}

void APBGameMode::TeamScoreHandleBeginPlay()
{
	APBGameState* GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		GS->UpdateTeamTargetScore(TargetScoreCache);
	}
}

void APBGameMode::OnTriggerEvent(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor)
{
	APBGameState* GS = GetGameState<APBGameState>();
	if (GS)
	{
		GS->NotifyTrigger(TriggerComp, TriggeredActor);
	}
}

void APBGameMode::OnGlobalEvent(FName EventName)
{
	APBGameState* GS = GetGameState<APBGameState>();
	if (GS)
	{
		GS->NotifyGlobalEvent(EventName);
	}
}

APBMission* APBGameMode::CreateMissionForTeam(EPBTeamType Team)
{
	UWorld* World = GetWorld();
	if (World && GameState)
	{
		APBMission* NewMission =
			Cast<APBMission>(UPBGameplayStatics::BeginDeferredActorSpawnFromClass(GameState, APBMission::StaticClass(), FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GameState));

		NewMission->Init(Team);

		UPBGameplayStatics::FinishSpawningActor(NewMission, FTransform::Identity);

		return NewMission;
	}

	return nullptr;
}

void APBGameMode::InitMissionSys()
{
	OnKilledEvt.AddUObject(this, &APBGameMode::NotifyMissionsOfKill);
	MatchTimeUpEvt.AddUObject(this, &APBGameMode::NotifyMissionOfTimeUp);
}

void APBGameMode::DecideMissionsBeforeWarmUp()
{
	APBGameState* GS = GetGameState<APBGameState>();
	if (GS)
	{
		for (uint32 i = 0; i < (uint32)EPBTeamType::Max; ++i)
		{
			EPBTeamType CurrentTeam = (EPBTeamType)i;
			APBMission* M = CreateMissionForTeam(CurrentTeam);
			if (M)
			{
				M->WantToEndMatchEvt.AddUObject(this, &APBGameMode::HandleEarlyMatchConclusion);
				GS->SetTeamMission(CurrentTeam, M);
			}
		}
	}
}

void APBGameMode::NotifyMissionsOfKill(AController* Killer, AController* Killed)
{
	APBGameState* GS = GetGameState<APBGameState>();
	APBPlayerState* KillerPS = Killer ? Cast<APBPlayerState>(Killer->PlayerState) : nullptr;
	APBPlayerState* KilledPS = Killed ? Cast<APBPlayerState>(Killed->PlayerState) : nullptr;
	if (GS && KillerPS && KilledPS)
	{
		GS->NotifyKill(KillerPS, KilledPS);
	}
}

void APBGameMode::NotifyMissionOfTimeUp()
{
	APBGameState* GS = GetGameState<APBGameState>();
	if (GS)
	{
		GS->NotifyMatchTimeup();
	}
}

void APBGameMode::HandleEarlyMatchConclusion()
{
	EndMatch();
}

void APBGameMode::ConcludeMissionAfterMatch()
{
	APBGameState* GS = GetGameState<APBGameState>();
	if (!GS)
	{
		return;
	}

	// Step 1: Decide Winning Team based on who has most match points
	TArray<EPBTeamType> WinningTeams;
	{
		int32 WinningMatchPoint = MIN_int32;
		for (uint32 i = 0; i < (uint32)EPBTeamType::Max; ++i)
		{
			EPBTeamType CurrentTeam = (EPBTeamType)i;
			APBMission* TeamMission = GS->GetTeamMission(CurrentTeam);
			if (TeamMission)
			{
				if (TeamMission->CurrentMatchPoint > WinningMatchPoint)
				{
					WinningMatchPoint = TeamMission->CurrentMatchPoint;
					WinningTeams.Empty();
					WinningTeams.Add(CurrentTeam);
				}
				else if (TeamMission->CurrentMatchPoint == WinningMatchPoint)
				{
					WinningTeams.Add(CurrentTeam);
				}
			}
		}
	}

	// Step 2: Notify player controllers
	if (GetWorld())		
	{
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			APBPlayerState* PlayerState = Cast<APBPlayerState>((*It)->PlayerState);
			bool bIsWinner = PlayerState && WinningTeams.Contains(PlayerState->GetTeamType());
			AActor* EndGameFocus = It->IsValid() ? (*It)->GetPawn() : NULL;
			(*It)->GameHasEnded(EndGameFocus, bIsWinner);
		}
	}	
}

bool APBGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// Always decide new starting spot when respawn
	if (Player && Player->GetPawn() == nullptr && Player->StartSpot != nullptr)
	{
		return true;
	}

	return false;
}

void APBGameMode::RestartPlayer(class AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	// NewPlayer 가 봇이면 스폰시킨다.
	SpawnBotAtStartTransform(NewPlayer);

	BroadcastSpawn(NewPlayer);
}

AActor* APBGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;
		if (TestSpawn->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
		else
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}

	if (BestStart == nullptr)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	if (BestStart == nullptr)
	{
		BestStart = Cast<APlayerStart>(Super::ChoosePlayerStart_Implementation(Player));
	}
	
	// if ShouldSpawnAtStartSpot == true, use Player->StartSpot when character spawn
	Player->StartSpot = BestStart;

	return BestStart;
}

bool APBGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player)
{
	APBPlayerStart* PBSpawnPoint = Cast<APBPlayerStart>(SpawnPoint);
	if (PBSpawnPoint)
	{
		return PBSpawnPoint->IsAllowed(Player);
	}
	else
	{
		return false;
	}
}

// 스폰할 위치에 다른 플레이어가 오버랩되어 있는지 
bool APBGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player)
{
	if (!SpawnPoint)
		return false;

	UClass* PawnClassToSpawn = GetDefaultPawnClassForController(Player);
	if (!PawnClassToSpawn)
		return false;

	ACharacter* PawnToSpawn = Cast<ACharacter>(PawnClassToSpawn->GetDefaultObject());
	if (!PawnToSpawn)
		return false;

	const FVector SpawnLocation = SpawnPoint->GetActorLocation();

	if (GetWorld())
	{
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != PawnToSpawn)
			{
				const float CombinedHeight = (PawnToSpawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = PawnToSpawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void APBGameMode::InitSpawnSys()
{
	UnbeatableTimeUponSpawn = 0.5f;
}

void APBGameMode::InitDamageModSys()
{
	DamageSelfScale = 0.f;
}

void APBGameMode::AddSpecifyWeaponsTo(APBCharacter* Pawn)
{
}

bool APBGameMode::CanDealDamage(APBPlayerState* DamageInstigator, APBPlayerState* DamagedPlayer) const
{
	return true;
}

float APBGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	APBCharacter* DamagedPawn = Cast<APBCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		APBPlayerState* DamagedPlayerState = Cast<APBPlayerState>(DamagedPawn->PlayerState);
		APBPlayerState* InstigatorPlayerState = Cast<APBPlayerState>(EventInstigator->PlayerState);

		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}

		if (InstigatorPlayerState == DamagedPlayerState)
		{
			ActualDamage *= DamageSelfScale;
		}
	}

	return ActualDamage;
}

void APBGameMode::ReportInGameWidgetClasses(APlayerController* NewPlayer)
{
	APBPlayerController* PBPlayer = Cast<APBPlayerController>(NewPlayer);
	if (PBPlayer)
	{
		PBPlayer->ClientSetInGameWidget(InGameWidgetClass);
	}
}

void APBGameMode::UpdateKillMark(AController* Killer, const TArray<AController*>& KilledPlayers, AActor* DamageCauser, const TArray<FTakeDamageInfo>& TakeDamageInfos)
{
	APBPlayerState* KillerPlayerState = Killer ? Cast<APBPlayerState>(Killer->PlayerState) : nullptr;
	if (KillerPlayerState)
	{
		KillerPlayerState->UpdateKillMark(Killer, KilledPlayers, DamageCauser, TakeDamageInfos);
	}
}

APBPickup* APBGameMode::NewPickupItem(TSubclassOf<APBPickup> WantsType, const FTransform& SpawnTransform)
{
	TArray<APBPickup*> Items;
	PickupItemMap.MultiFind(WantsType, Items);

	// Find in the Cached Item list first.
	for (auto Item : Items)
	{
		if (Item && !Item->IsActive())
		{
			Item->SetActorTransform(SpawnTransform);
			return Item;
		}
	}

	// if you couldn't find pickup in the cached list, then create a instance
	APBPickup* SpawnInstance = GetWorld() ? GetWorld()->SpawnActor<APBPickup>(WantsType, SpawnTransform) : nullptr;
	if (SpawnInstance)
	{
		PickupItemMap.Add(WantsType, SpawnInstance);
	}

	return SpawnInstance;
}

class APBPickup* APBGameMode::NewPickupWeap(class APBWeapon* InWeap, const FTransform& SpawnTransform)
{
	if (false == (InWeap && PickupWeapBaseClass))
	{
		return nullptr;
	}

	// if you couldn't find pickup in the cached list, then create a instance
	APBPickupWeapon* SpawnInstance = GetWorld() ? GetWorld()->SpawnActorDeferred<APBPickupWeapon>(PickupWeapBaseClass, SpawnTransform) : nullptr;
	if (SpawnInstance)
	{
		SpawnInstance->MulticastSetPickupContentsFromWeapon(InWeap);
		SpawnInstance->SetMaxSpawnNum(1);
		SpawnInstance->SetDestroyDelay(PickupDestroyDelay);
		SpawnInstance->CurrentAmmoInClip = InWeap->GetCurrentAmmoInClip();
		SpawnInstance->CurrentAmmo = InWeap->GetCurrentAmmo();

		UGameplayStatics::FinishSpawningActor(SpawnInstance, SpawnTransform);
	}

	return SpawnInstance;


}

void APBGameMode::InitPickup()
{
	ConstructorHelpers::FClassFinder<class APBPickupWeapon> PickupBaseClassFinder(TEXT("/Game/Blueprints/GameMode/Pickups/PickupItems/Weapon/PickupWeapBase"));
	if (PickupBaseClassFinder.Succeeded())
	{
		PickupWeapBaseClass	= PickupBaseClassFinder.Class;
	}
	else
	{
		UE_LOG(LogPB, Warning, TEXT(" APBGameMode::InitPickup() Fail to load base pickup weapon class"));
	}

	PickupDestroyDelay = 0.0f;
}

UClass* APBGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// Bot
	APBAIController* BotController = Cast<APBAIController>(InController);
	if (BotController && BotController->GetDefaultPawnClass())
	{
		return BotController->GetDefaultPawnClass();
	}

	if (BotController && BotPawnClass != nullptr)
	{
		return BotPawnClass;
	}

	if (GetWorld() && !GetWorld()->IsPlayInEditor())
	{
		APBPlayerState* PS = Cast<APBPlayerState>(InController->PlayerState);
		if (PS)
		{
			uint32 CharItemID = 0;
			int32 TeamIdx = int32(PS->GetTeamType());

			if (PS->ReplicateEquippedCharItems.IsValidIndex(TeamIdx))
			{
				CharItemID = PS->ReplicateEquippedCharItems[TeamIdx];
			}

			if (CharItemID > 0)
			{
				UPBItemTableManager* TM = UPBItemTableManager::Get(this);
				if (TM)
				{
					FPBCharacterTableData* Data = TM->GetCharacterTableData(CharItemID);
					if (Data)
					{
						check(TeamIdx == Data->TeamEnum);

						if (ensure(Data->BPClass != nullptr))
						{
							return Data->BPClass;
						}
					}
				}
			}
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void APBGameMode::InitBotSys()
{
	TargetBotNum = 0;
}

void APBGameMode::BotSysHandleInitGame(const FString& Options)
{
	const int32 BotsCountOptionValue = UPBGameplayStatics::GetIntOption(Options, GetBotsCountOptionName(), 0);
	TargetBotNum = FMath::Min(BotsCountOptionValue, 8);
}

FString APBGameMode::GetBotsCountOptionName()
{
	return FString(TEXT("Bots"));
}

void APBGameMode::BotSysHandleWarmUp()
{
	UWorld* World = GetWorld();
	if (World)
	{
		int32 ExistingBots = 0;
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
		{
			APBAIController* AIC = Cast<APBAIController>(*It);
			if (AIC)
			{
				AIC->bLevelBot = true;
				InitBot(AIC, ExistingBots);
				++ExistingBots;
			}
		}

		int32 BotNum = ExistingBots;
		for (int32 i = 0; i < TargetBotNum - BotNum; ++i)
		{
			CreateBot(BotNum + i);
		}
	}
}

APBAIController* APBGameMode::CreateBot(int32 BotNum)
{
	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	APBAIController* AIC = BotControllerClass ? World->SpawnActor<APBAIController>(BotControllerClass, SpawnInfo) : World->SpawnActor<APBAIController>(SpawnInfo);
	InitBot(AIC, BotNum);

	return AIC;
}

void APBGameMode::InitBot(APBAIController* AIController, int32 BotNum)
{
	APBPlayerState* BotPlayerState = CastChecked<APBPlayerState>(AIController->PlayerState);
	EPBTeamType TeamNum = ChooseTeam(BotPlayerState);
	BotPlayerState->SetTeamType(TeamNum);

	if (AIController)
	{
		if (AIController->PlayerState)
		{
			FString BotName = FString::Printf(TEXT("Bot %d"), BotNum);
			AIController->PlayerState->PlayerName = BotName;
		}
	}
}

void APBGameMode::BotSysHandleMatchStart()
{
	UWorld* World = GetWorld();
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		APBAIController* AIC = Cast<APBAIController>(*It);
		if (AIC)
		{
			RestartPlayer(AIC);
		}
	}
}

void APBGameMode::SpawnBotAtStartTransform(AController* BotController)
{
	APBAIController* PC = Cast<APBAIController>(BotController);
	if (PC && PC->bLevelBot)
	{
		FVector StartLocation = PC->GetStartLocation();
		PC->ClientSetLocation(StartLocation, PC->GetPawn()->GetActorRotation());

		FRotator NewControllerRot = PC->GetStartRotation();
		NewControllerRot.Roll = 0.f;
		PC->SetControlRotation(NewControllerRot);
	}
}

bool APBGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

UPBGameTableManager* APBGameMode::GetGameTableManager() const
{
	UPBGameInstance* GameInstance = Cast<UPBGameInstance>(GetGameInstance());
	return GameInstance ? GameInstance->GetGameTableManager() : nullptr;
}

void APBGameMode::InitTableSys()
{
	ItemID = 0;
	UseTableDataToConfig = true;
}

bool APBGameMode::ApplyTableData(const struct FPBGameModeTableData* Data)
{
	if (!GetUseTableTata())
		return false;

	// Todo

	return true;
}

void APBGameMode::Tick(float DeltaSeconds)
{
	// InProgress (게임 중) 상태 일 때 플레이어 수를 체크하여 플레이어 수가 0이면 WaitingToStart(대기) 상태로 바꾼다.
	auto GS = GetGameState<APBGameState>();

	if (GS)
	{
		if (GS->GetMatchState() == MatchState::InProgress)
		{
			if (GS->PlayerArray.Num() == 0)
			{
				SetMatchState(MatchState::WaitingToStart);
				GS->SetMatchState(MatchState::WaitingToStart);
			}
		}
	}
}

FPBGameModeTableData* APBGameMode::GetTableData()
{
	if (GetItemID() <= 0)
		return nullptr;

	UPBGameTableManager* TableManager = UPBGameTableManager::Get(this);
	if (TableManager)
	{
		UPBGameModeTable* Table = Cast<UPBGameModeTable>(TableManager->GetTable(EPBTableType::GameMode));
		if (Table)
		{
			return Table->GetData(GetItemID());
		}
	}

	return nullptr;
}