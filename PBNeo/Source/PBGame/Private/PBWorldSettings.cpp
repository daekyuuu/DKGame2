// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWorldSettings.h"
#include "PBGameInstance.h"

APBWorldSettings::APBWorldSettings()
{
	BGMFadeInTime = 1.f;
	BGMFadeOutTime = 1.f;

	TeamColors.Add(FColor(229, 61, 61)); // Alpha, Red
	TeamColors.Add(FColor(2, 145, 255)); // Bravo, Blue
}

void APBWorldSettings::NotifyBeginPlay()
{
	Super::NotifyBeginPlay();
}

void APBWorldSettings::PlayBGM()
{
	if (BGM != nullptr && GetNetMode() != NM_DedicatedServer)
	{
		if (BGMComp == nullptr)
		{
			BGMComp = NewObject<UAudioComponent>(this);
			BGMComp->bAllowSpatialization = false;
			BGMComp->bShouldRemainActiveIfDropped = true;
			BGMComp->SetSound(BGM);
		}

		if (BGMComp)
		{
			if (BGMComp->IsPlaying())
			{
				BGMComp->FadeIn(BGMFadeInTime);
			}
			else
			{
				BGMComp->Play();
			}
		}
	}
}

void APBWorldSettings::StopBGM()
{
	if (BGMComp && BGMComp->IsPlaying() && !BGMComp->IsPendingKill())
	{
		BGMComp->Stop();
	}
}

void APBWorldSettings::FadeOutBGM()
{
	if (BGMComp && BGMComp->IsPlaying())
	{
		BGMComp->FadeOut(BGMFadeOutTime, 0.f);
	}
}

FColor APBWorldSettings::GetTeamColor(EPBTeamType TeamType)
{
	int32 TeamTypeAsIndex = (int32)(TeamType);
	if (false == TeamColors.IsValidIndex(TeamTypeAsIndex))
		return FColor::White;
	
	return TeamColors[TeamTypeAsIndex];
}