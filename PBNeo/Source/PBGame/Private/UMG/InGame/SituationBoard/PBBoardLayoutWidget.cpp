// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBoardLayoutWidget.h"
#include "PBGameState.h"
#include "PBPlayerState.h"



void UPBBoardLayoutWidget::UpdateTeamPlayerArray(EPBTeamType teamType)
{
	auto gs = GetPBGameState();
	if (false == IsSafe(gs))
	{
		return;
	}

	TeamPlayerArray.Empty();
	TeamPlayerArray = gs->GetTeamPlayers((int32)teamType);
}

class APBPlayerState* UPBBoardLayoutWidget::GetTeamPlayerState(int32 index) const
{
	if (TeamPlayerArray.IsValidIndex(index))
	{
		return TeamPlayerArray[index];
	}

	return nullptr;
}

int32 UPBBoardLayoutWidget::GetTeamScoreNum(EPBTeamType Team) const
{
	auto gs = GetPBGameState();
	if (gs)
	{
		return gs->GetTeamScore(Team);
	}

	return 0;
}

