// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBInGameWidget.h"
#include "Mode/PBGameMode.h"

UPBInGameWidget::UPBInGameWidget()
{
	CurrState = EPBInGameWidgetState::WaitingToStart;
}

FText UPBInGameWidget::GetRamainingTime() const
{
	FString TimeString;

	if (EPBInGameWidgetState::WaitingToStart == GetState())
	{
		TimeString = "Warm Up Time: ";
	}
	else if (EPBInGameWidgetState::WaitingPostMatch == GetState())
	{
		TimeString = "To Next Match: ";
	}
	else if (EPBInGameWidgetState::WaitingConcludeAllMatches == GetState())
	{
		TimeString = "To Main Menu: ";
	}

	APBGameState* const GameState = GetPBGameState();
	if (GameState)
	{
		int32 time = FMath::Max(GameState->RemainingTime, 0);
		TimeString += (FString::FromInt(time));
	}

	return FText::FromString(TimeString);
}

void UPBInGameWidget::SetState(FName InMatchState, uint8 InMatchSubstateIndex)
{
	if (InMatchState == MatchState::WaitingToStart)
	{
		CurrState = EPBInGameWidgetState::WaitingToStart;
	}
	else if (InMatchState == MatchState::InProgress)
	{
		CurrState = EPBInGameWidgetState::MatchInProgress;
	}
	else if (InMatchState == MatchState::WaitingPostMatch)
	{
		if ((EPBPostMatchState)InMatchSubstateIndex == EPBPostMatchState::ConcludeAllMatches)
		{
			CurrState = EPBInGameWidgetState::WaitingConcludeAllMatches;
		}
		else
		{
			CurrState = EPBInGameWidgetState::WaitingPostMatch;
		}
	}
	else
	{
		CurrState = EPBInGameWidgetState::None;
	}

	OnStateHasChanged(CurrState);
}

EPBInGameWidgetState UPBInGameWidget::GetState() const
{
	return CurrState;
}
