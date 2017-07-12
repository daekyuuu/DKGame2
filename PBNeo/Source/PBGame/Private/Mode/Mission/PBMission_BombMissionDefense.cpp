// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMission_BombMissionDefense.h"
#include "PBPlayerState.h"
#include "PBProj_Bomb.h"

APBMission_BombMissionDefense::APBMission_BombMissionDefense()
{
	bPlayerCanRespawn = false;

	bBombPlanted = false;
}

void APBMission_BombMissionDefense::NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed)
{
	if (Killer && Killer->GetTeamType() == OwningTeam)
	{
		if (!bBombPlanted)
		{
			if (!GetWorld())
				return;

			APBGameState* GS = Cast<APBGameState>(GetWorld()->GetGameState());
			if (!GS)
				return;

			for (APlayerState*& PS : GS->PlayerArray)
			{
				APBPlayerState* PBPS = Cast<APBPlayerState>(PS);
				if (PBPS)
				{
					if (PBPS->GetTeamType() != OwningTeam && !PBPS->DiedAtLeastOnceInMatch() && PBPS != Killed)
					{
						return;
					}
				}
			}

			if (CurrentMatchPoint == 0)
			{
				++CurrentMatchPoint;
				TeamScoredEvt.Broadcast(this, 1);
				WantToEndMatchEvt.Broadcast();
			}
		}
	}
}

void APBMission_BombMissionDefense::NotifyGlobalEvent(FName EventName)
{
	if (EventName == FName(TEXT("BombPlanted")))
	{
		bBombPlanted = true;
	}

	if (EventName == FName(TEXT("BombDefused")))
	{
		if (CurrentMatchPoint == 0)
		{
			++CurrentMatchPoint;
			TeamScoredEvt.Broadcast(this, 1);
			WantToEndMatchEvt.Broadcast();
		}
	}
}

void APBMission_BombMissionDefense::NotifyMatchTimeup()
{
	if (CurrentMatchPoint == 0)
	{
		++CurrentMatchPoint;
		TeamScoredEvt.Broadcast(this, 1);

		for (TActorIterator<APBProj_Bomb> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APBProj_Bomb* Bomb = *ActorItr;
			if (Bomb)
			{
				Bomb->FreezeBomb();
			}
		}
	}
}

bool APBMission_BombMissionDefense::CanDefuseBomb()
{
	return true;
}
