// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPlayerState.h"
#include "PBGameInstance.h"
#include "PBMission.h"

APBGameState::APBGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentRound = 0;

	InitScorePerTeam();
	InitMatchCycleSys();

	// Default to every few seconds.
	ServerWorldTimeSecondsUpdateFrequency = 1.0f;

	BombRemainingTime = 0;
}

void APBGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBGameState, GameModeType);

	DOREPLIFETIME(APBGameState, bTimerPaused);
	DOREPLIFETIME(APBGameState, RemainingTime);

	DOREPLIFETIME(APBGameState, TeamScore);
	DOREPLIFETIME(APBGameState, TargetTeamScore);

	DOREPLIFETIME(APBGameState, TeamMissions);

	DOREPLIFETIME(APBGameState, CurrentRound);

	DOREPLIFETIME(APBGameState, BombRemainingTime);
}

void APBGameState::Reset()
{
	Super::Reset();
	ResetMissionSys();
}

void APBGameState::UpdateServerTimeSeconds()
{
	Super::UpdateServerTimeSeconds();

	auto World = GetWorld();
	if (nullptr == World)
		return;

	//if (false == IsNetMode(ENetMode::NM_DedicatedServer))
	if (Role == ROLE_Authority)
	{
		if (BombRemainingTime > 0)
		{
			ServerSetBombRemainingTime(FMath::Max(BombRemainingTime - 1.0f, 0.0f));
		}
	}
}

void APBGameState::OnRep_CurrentRound()
{
	auto World = GetWorld();
	if (nullptr == World)
		return;

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC && PC->IsLocalController())
		{
			PC->WidgetEventDispatcher->OnMatchStarted.Broadcast(CurrentRound);
			PC->WidgetEventDispatcher->OnConnectionTypeChanged.Broadcast(nullptr != AuthorityGameMode);

			auto OC = Cast<APBCharacter>(PC->GetPawn());
			if (OC)
			{
				OC->PlaySoundMissionStart();
			}
		}
	}
}

void APBGameState::IncreaseRound()
{
	++CurrentRound;
}

bool APBGameState::ServerIncreaseRound_Validate()
{
	return true;
}

void APBGameState::ServerIncreaseRound_Implementation()
{
	IncreaseRound();
	OnRep_CurrentRound();
}

void APBGameState::InitMatchCycleSys()
{
	bTimerPaused = false;
	RemainingTime = 0;
}

void APBGameState::InitScorePerTeam()
{
	TargetTeamScore = 1;
}

void APBGameState::AddScoreToTeam(EPBTeamType TeamType, uint32 Point)
{
	if (HasAuthority())
	{
		int32 TeamNumber = (int32)TeamType;

		if (TeamNumber >= 0)
		{
			if (TeamNumber >= TeamScore.Num())
			{
				TeamScore.AddZeroed(TeamNumber - TeamScore.Num() + 1);
			}

			TeamScore[TeamNumber] += Point;
		}
	}
}

int32 APBGameState::GetTeamScore(EPBTeamType TeamType) const
{
	int32 Index = (int32)TeamType;

	if (TeamScore.IsValidIndex(Index))
	{
		return TeamScore[Index];
	}

	return 0;
}

bool APBGameState::HasAnyAlivePlayer(EPBTeamType TeamType) const
{
	TArray<APBPlayerState *> PlayerStates = GetTeamPlayers((int32)TeamType);
	for (auto CurPS : PlayerStates)
	{
		auto CurOC = CurPS->GetOwnerCharacter();
		if (CurOC && CurOC->IsAlive() && false == CurOC->IsSpectator())
			return true;
	}

	return false;
}

void APBGameState::UpdateTeamTargetScore(uint32 Point)
{
	TargetTeamScore = Point;
}

int32 APBGameState::GetTeamTargetScore() const
{
	return TargetTeamScore;
}

void APBGameState::SetTeamMission(EPBTeamType Team, APBMission* Mission)
{
	if (HasAuthority() && Mission)
	{
		int32 TeamNumber = (int32)Team;
		if (TeamNumber >= 0)
		{
			if (TeamNumber >= TeamMissions.Num())
			{
				TeamMissions.AddDefaulted(TeamNumber - TeamMissions.Num() + 1);
			}

			if (TeamMissions[TeamNumber])
			{
				TeamMissions[TeamNumber]->Destroy();
				TeamMissions[TeamNumber] = nullptr;
			}

			Mission->TeamScoredEvt.AddUObject(this, &APBGameState::OnMissionScoredPoint);
			TeamMissions[TeamNumber] = Mission;
		}
	}
}

APBMission* APBGameState::GetTeamMission(EPBTeamType Team)
{
	int32 TeamNumber = (int32)Team;
	if (TeamMissions.IsValidIndex(TeamNumber))
	{
		return TeamMissions[TeamNumber];
	}

	return nullptr;
}

void APBGameState::NotifyKill(APlayerState* Killer, APlayerState* Killed)
{
	for (APBMission* Mission : TeamMissions)
	{
		if (Mission != nullptr)
		{
			Mission->NotifyKill(Cast<APBPlayerState>(Killer), Cast<APBPlayerState>(Killed));
		}
	}
}

void APBGameState::NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor)
{
	for (APBMission* Mission : TeamMissions)
	{
		if (Mission != nullptr)
		{
			Mission->NotifyTrigger(TriggerComp, TriggeredActor);
		}
	}
}

void APBGameState::NotifyGlobalEvent(FName EventName)
{
	for (APBMission* Mission : TeamMissions)
	{
		if (Mission != nullptr)
		{
			Mission->NotifyGlobalEvent(EventName);
		}
	}
}

void APBGameState::NotifyMatchTimeup()
{
	for (APBMission* Mission : TeamMissions)
	{
		if (Mission != nullptr)
		{
			Mission->NotifyMatchTimeup();
		}
	}
}

int32 APBGameState::GetMatchPointOfTeam(EPBTeamType Team)
{
	APBMission* Mission = GetTeamMission(Team);
	if (Mission)
	{
		return Mission->CurrentMatchPoint;
	}

	return 0;
}

void APBGameState::ResetMissionSys()
{
	for (APBMission* Mission : TeamMissions)
	{
		if (Mission != nullptr)
		{
			Mission->Destroy();
		}
	}

	TeamMissions.Reset();

	if (Role == ROLE_Authority)
	{
		ServerSetBombRemainingTime(0);
	}
}

void APBGameState::OnMissionScoredPoint(APBMission* Mission, uint32 Point)
{
	AddScoreToTeam(Mission->OwningTeam, Point);
}

TArray<class APBPlayerState*> APBGameState::GetTeamPlayers(int32 TeamIndex) const
{
	TArray<class APBPlayerState*> players;
	for (int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		APBPlayerState* CurPlayerState = Cast<APBPlayerState>(PlayerArray[i]);
		if (CurPlayerState && (int32(CurPlayerState->GetTeamType()) == TeamIndex))
		{
			players.Add(CurPlayerState);
		}
	}

	return players;
}

void APBGameState::GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap, bool bSortAsGreater) const
{
	OutRankedMap.Empty();

	//first, we need to go over all the PlayerStates, grab their score, and rank them
	TMultiMap<int32, APBPlayerState*> SortedMap;
	for (int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		int32 Score = 0;
		APBPlayerState* CurPlayerState = Cast<APBPlayerState>(PlayerArray[i]);
		if (CurPlayerState && (int32(CurPlayerState->GetTeamType()) == TeamIndex))
		{
			SortedMap.Add(FMath::TruncToInt(CurPlayerState->GetSquadSortValue()), CurPlayerState);
		}
	}

	//sort by the keys
	if (bSortAsGreater)
	{
		SortedMap.KeySort(TGreater<int32>());
	}
	else
	{
		SortedMap.KeySort(TLess<int32>());
	}

	//now, add them back to the ranked map
	OutRankedMap.Empty();

	int32 Rank = 0;
	for (TMultiMap<int32, APBPlayerState*>::TIterator It(SortedMap); It; ++It)
	{
		OutRankedMap.Add(Rank++, It.Value());
	}

}

bool APBGameState::ServerSetBombRemainingTime_Validate(float InBombRemainingTime)
{
	return true;
}

void APBGameState::ServerSetBombRemainingTime_Implementation(float InBombRemainingTime)
{
	BombRemainingTime = InBombRemainingTime;
	OnRep_BombRemainingTime();
}

void APBGameState::OnRep_BombRemainingTime()
{
	auto World = GetWorld();
	if (nullptr == World)
		return;

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC && PC->IsLocalController())
		{
			PC->WidgetEventDispatcher->OnSyncBombRemainingTime.Broadcast(BombRemainingTime);
		}
	}
}

