// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBScoreAndTimerWidget.h"
#include "PBUMGUtils.h"
#include "PBGameState.h"
#include "PBPlayerState.h"
#include "PBCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBScoreAndTimerWidget, Round);

void UPBScoreAndTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnDeath, UPBScoreAndTimerWidget::NotifyDeath);
	BindWidgetEvent(OnSpawn, UPBScoreAndTimerWidget::NotifySpawn);
	BindWidgetEvent(OnMatchStarted, UPBScoreAndTimerWidget::NotifyMatchStartedEvent);

	GetBPV_Round()->SetText(FText::FromString("1R"));
}

void UPBScoreAndTimerWidget::NativeDestruct()
{

	UnbindWidgetEvent(OnMatchStarted);
	UnbindWidgetEvent(OnSpawn);
	UnbindWidgetEvent(OnDeath);

	Super::NativeDestruct();
}

void UPBScoreAndTimerWidget::NotifyMatchStartedEvent(int32 Round)
{
	GetBPV_Round()->SetText(FText::FromString(FString::FromInt(Round) + TEXT("R")));
}

FText UPBScoreAndTimerWidget::GetMatchTimeText() const
{
	FString TimeString("00:00");

	APBGameState* const GameState = GetPBGameState();

	if (GameState)
	{
		TimeString = UPBUMGUtils::ToTimeString(GameState->RemainingTime);
	}

	return FText::FromString(TimeString);
}

int32 UPBScoreAndTimerWidget::GetMatchTime() const
{
	if (GetPBGameState())
	{
		return GetPBGameState()->RemainingTime;
	}
	return 0;

}

FText UPBScoreAndTimerWidget::GetTeamScoreText(EPBTeamType Team) const
{
	return FText::AsNumber(GetTeamScoreNum(Team));
}

int32 UPBScoreAndTimerWidget::GetTeamScoreNum(EPBTeamType Team) const
{
	auto gs = GetPBGameState();
	if (gs)
	{
		return gs->GetTeamScore(Team);
	}

	return 0;
}

EPBTeamType UPBScoreAndTimerWidget::GetTeamType() const
{
	auto ps = GetPBPlayerState();
	if (false == IsSafe(ps))
	{
		return EPBTeamType::Max;
	}

	return ps->GetTeamType();

}

float UPBScoreAndTimerWidget::GetWinPercentage(EPBTeamType TeamType) const
{

	auto GS = GetPBGameState();
	if (GS)
	{
		float TeamScore = (float)(GS->GetTeamScore(TeamType));
		float TargetScore = (float)(GS->GetTeamTargetScore());

		if (false == FMath::IsNearlyZero(TargetScore))
		{
			return TeamScore / TargetScore;
		}

	}

	
	return 1.0f;


}

TArray<bool> UPBScoreAndTimerWidget::GetTeamAlivilities(EPBTeamType TeamType) const
{
	TArray<bool> Alivilities;
	auto GS = GetPBGameState();
	int32 DeadCnt = 0;

	if (GS)
	{
		auto PlayerStateArray = GS->GetTeamPlayers((int32)TeamType);
		for (auto PS : PlayerStateArray)
		{
			if (PS)
			{
				auto PBCharacter = PS->GetOwnerCharacter();
				if (PBCharacter && PBCharacter->IsAlive())
				{
					Alivilities.Add(true);
				}
				else
				{
					++DeadCnt;
				}
			}
		}
	}

	for (int i = 0; i < DeadCnt; ++i)
	{
		Alivilities.Add(false);
	}

	return Alivilities;
}
