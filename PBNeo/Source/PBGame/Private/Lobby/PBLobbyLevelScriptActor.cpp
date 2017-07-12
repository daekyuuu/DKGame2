// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbyLevelScriptActor.h"
#include "PBLobby.h"


void APBLobbyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	InitLights();

}

void APBLobbyLevelScriptActor::InitLights()
{

	LightsOfPages.Empty();
	LightsOfPages.Add(FLobbyCamName::MainMenu);
	LightsOfPages.Add(FLobbyCamName::Multiplay);
	LightsOfPages.Add(FLobbyCamName::CharSelect_Main);
	LightsOfPages.Add(FLobbyCamName::CharSelect_Left);
	LightsOfPages.Add(FLobbyCamName::CharSelect_Right);
	LightsOfPages.Add(FLobbyCamName::LoadingScreen);


	BrightnessOfLights.Empty();

	// Get lights from the level
	for (TActorIterator<ALight> Iter(GetWorld()); Iter; ++Iter)
	{
		if (Iter->IsValidLowLevel())
		{
			if (Iter->ActorHasTag(FLobbyCamName::MainMenu))
			{
				LightsOfPages[FLobbyCamName::MainMenu].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());
			}
			else if (Iter->ActorHasTag(FLobbyCamName::Multiplay))
			{
				LightsOfPages[FLobbyCamName::Multiplay].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());

			}
			else if (Iter->ActorHasTag(FLobbyCamName::CharSelect_Main))
			{
				LightsOfPages[FLobbyCamName::CharSelect_Main].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());

			}
			else if (Iter->ActorHasTag(FLobbyCamName::CharSelect_Left))
			{
				LightsOfPages[FLobbyCamName::CharSelect_Left].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());

			}
			else if (Iter->ActorHasTag(FLobbyCamName::CharSelect_Right))
			{
				LightsOfPages[FLobbyCamName::CharSelect_Right].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());
			}
			else if (Iter->ActorHasTag(FLobbyCamName::LoadingScreen))
			{
				LightsOfPages[FLobbyCamName::LoadingScreen].Add(*Iter);
				BrightnessOfLights.Add(*Iter, Iter->GetBrightness());
			}
		}
	}
}

void APBLobbyLevelScriptActor::TurnOffEveryLights()
{
	for (auto Lights : LightsOfPages)
	{
		for (auto Light : Lights.Value)
		{
			Light->GetLightComponent()->SetVisibility(false);
		}
	}
}

void APBLobbyLevelScriptActor::TurnOnAndSetBrightnessRateOfLights(const FName& PageName, float BrightnessRate)
{
	TurnOnLights(PageName);
	SetBrightnessRateOfLights(PageName, BrightnessRate);
}

void APBLobbyLevelScriptActor::TurnOnLights(const FName& PageName)
{
	if (LightsOfPages.Contains(PageName))
	{
		auto Lights = LightsOfPages[PageName];
		for (auto Light : Lights)
		{
			Light->GetLightComponent()->SetVisibility(true);
		}
	}
}

void APBLobbyLevelScriptActor::SetBrightnessRateOfLights(const FName& PageName, float BrightnessRate)
{
	if (false == LightsOfPages.Contains(PageName))
	{
		return;
	}

	auto Lights = LightsOfPages[PageName];
	float const Rate = FMath::Clamp(BrightnessRate, 0.0f, 1.0f);

	for (auto Light : Lights)
	{
		if (BrightnessOfLights.Contains(Light))
		{
			// Initial Brightness can't be zero
			float const Max = FMath::Max(BrightnessOfLights[Light], 1.0f);
			float const Brightness = Max * Rate;
			Light->SetBrightness(Brightness);

		}
	}




}
