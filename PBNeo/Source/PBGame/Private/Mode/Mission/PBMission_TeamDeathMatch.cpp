// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMission_TeamDeathMatch.h"
#include "PBPlayerState.h"

APBMission_TeamDeathMatch::APBMission_TeamDeathMatch()
{
	KillMatchPoint = 1;
	DeathMatchPoint = 0;
	TargetMatchPoint = 1;
}

void APBMission_TeamDeathMatch::NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed)
{
	uint32 MatchPointToAdd(0);

	if (Killer && Killer->GetTeamType() == OwningTeam && Killed && Killer != Killed)
	{
		MatchPointToAdd += KillMatchPoint;
	}

	if (Killed && Killed->GetTeamType() == OwningTeam)
	{
		MatchPointToAdd += DeathMatchPoint;
	}

	CurrentMatchPoint += MatchPointToAdd;

	if (MatchPointToAdd > 0)
	{
		TeamScoredEvt.Broadcast(this, MatchPointToAdd);
	}

	if (CurrentMatchPoint >= TargetMatchPoint)
	{
		WantToEndMatchEvt.Broadcast();
	}
}