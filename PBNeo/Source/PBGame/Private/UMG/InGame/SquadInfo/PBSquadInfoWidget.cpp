// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSquadInfoWidget.h"
#include "PBGameState.h"
#include "PBPlayerState.h"



void UPBSquadInfoWidget::UpdateRankedSquadMembers()
{
	auto gs = GetPBGameState();
	if (false == IsSafe(gs))
	{
		return;
	}

	auto ps = GetPBPlayerState();
	if (false == IsSafe(ps))
	{
		return;
	}


	RankedPlayerMap TempMap;
	TempMap.Empty();

	// Get local player's squadInfo
	int32 teamTypeIndex = (int32)ps->GetTeamType();
	gs->GetRankedMap(teamTypeIndex, TempMap, false);

	RankedSquadMembers = TempMap;


}

class APBPlayerState* UPBSquadInfoWidget::GetRankedSquadMember(int32 RankNum) const
{
	if (RankedSquadMembers.Contains(RankNum))
	{
		if (RankedSquadMembers[RankNum].IsValid())
		{
			return RankedSquadMembers[RankNum].Get();
		}
	}

	return nullptr;
}
