// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPlayerInfoAccessComponent.h"

#include "PBPlayerState.h"


FText UPBPlayerInfoAccessComponent::GetUserName() const
{
	FText username = FText::FromString("UserNameUserName");

	if (IsSafe(TargetPlayerState))
	{
		username = FText::FromString(TargetPlayerState->GetShortPlayerName());
	}

	return username;
}

float UPBPlayerInfoAccessComponent::GetHP() const
{
	float percentage = 0.0f;
	if (IsSafe(TargetPlayerState))
	{
		if (IsSafe(TargetPlayerState->OwnerCharacter))
		{
			percentage = TargetPlayerState->OwnerCharacter->GetHealthPercentage();
		}
	}

	return percentage;
}

FText UPBPlayerInfoAccessComponent::GetKill() const
{
	int32 killCnt = -1;

	if (IsSafe(TargetPlayerState))
	{
		killCnt = TargetPlayerState->GetKills();
	}

	return IntToText(killCnt);
}

FText UPBPlayerInfoAccessComponent::GetKillAndDeath() const
{
	FString Head = GetKill().ToString();
	FString Tail = GetDeath().ToString();

	FString Res = Head + " / " + Tail;

	return FText::FromString(Res);

}

FText UPBPlayerInfoAccessComponent::GetAssist() const
{
	// to do
	return IntToText(0);
}

FText UPBPlayerInfoAccessComponent::GetDeath() const
{
	int32 deathCnt = -1;

	if (IsSafe(TargetPlayerState))
	{
		deathCnt = TargetPlayerState->GetDeaths();
	}

	return IntToText(deathCnt);
}

FText UPBPlayerInfoAccessComponent::GetScore() const
{
	int32 scoreCnt = -1;

	if (IsSafe(TargetPlayerState))
	{
		scoreCnt = TargetPlayerState->GetScore();
	}

	return IntToText(scoreCnt);
}

bool UPBPlayerInfoAccessComponent::IsItLocalPlayerInfo() const
{
	if (nullptr != LocalPlayerController && nullptr != TargetPlayerState)
	{
		return GetLocalPlayerState() == TargetPlayerState;
	}

	return false;

}

bool UPBPlayerInfoAccessComponent::IsDie() const
{
	if (IsSafe(TargetPlayerState))
	{
		if (IsSafe(TargetPlayerState->OwnerCharacter))
		{
			return TargetPlayerState->OwnerCharacter->GetHealth() <= 0.0f;
		}
	}

	return false;
}

APBPlayerState* UPBPlayerInfoAccessComponent::GetLocalPlayerState() const
{
	return LocalPlayerController ? Cast<APBPlayerState>(LocalPlayerController->PlayerState) : nullptr;
}
