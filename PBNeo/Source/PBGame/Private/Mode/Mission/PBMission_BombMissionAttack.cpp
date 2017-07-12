// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMission_BombMissionAttack.h"
#include "PBPlayerState.h"
#include "PBTrigger_BombSite.h"
#include "PBProj_Bomb.h"
#include "Mode/PBLevelScriptActor.h"
#include "LevelSequencePlayer.h"

APBMission_BombMissionAttack::APBMission_BombMissionAttack()
{
	bPlayerCanRespawn = false;

	bBombPlanted = false;
}

void APBMission_BombMissionAttack::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBMission_BombMissionAttack, bBombPlanted);
}

void APBMission_BombMissionAttack::NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed)
{
	if (Killer && Killer->GetTeamType() == OwningTeam)
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
}

void APBMission_BombMissionAttack::NotifyGlobalEvent(FName EventName)
{
	auto World = GetWorld();
	if (false == ensure(nullptr != World))
		return;

	if (EventName == FName(TEXT("BombPlanted")))
	{
		bBombPlanted = true;

		for (TActorIterator<APBTrigger_BombSite> ActorItr(World); ActorItr; ++ActorItr)
		{
			APBTrigger_BombSite* BombSite = *ActorItr;
			if (BombSite)
			{
				BombSite->NotifyBombPlanted();
			}
		}
	}

	if (EventName == FName(TEXT("BombExploded")))
	{
		auto LSA = Cast<APBLevelScriptActor>(World->GetLevelScriptActor());
		if (LSA)
		{
			OnDisplayResultHandle = LSA->OnDisplayResult.AddUObject(this, &APBMission_BombMissionAttack::OnDisplayResult);

			if (nullptr == LSA->GetLevelSequence(TEXT("BombExploded")))
			{
				OnDisplayResult();
			}
			else
			{
				LSA->MulticastPlayLevelSequence(TEXT("BombExploded"));
			}
		}
		else
		{
			OnDisplayResult();
		}
	}
}

bool APBMission_BombMissionAttack::CanPlantBomb()
{
	return !bBombPlanted;
}

void APBMission_BombMissionAttack::OnDisplayResult()
{
	auto World = GetWorld();
	if (false == ensure(nullptr != World))
		return;

	auto LSA = Cast<APBLevelScriptActor>(World->GetLevelScriptActor());
	if (LSA)
	{
		LSA->OnDisplayResult.Remove(OnDisplayResultHandle);
	}

	if (CurrentMatchPoint == 0)
	{
		++CurrentMatchPoint;
		TeamScoredEvt.Broadcast(this, 1);
		WantToEndMatchEvt.Broadcast();
	}
}