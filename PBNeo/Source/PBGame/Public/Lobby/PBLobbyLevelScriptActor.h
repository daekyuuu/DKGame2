// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/PBLevelScriptActor.h"
#include "PBLobbyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBLobbyLevelScriptActor : public APBLevelScriptActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/* ------------------------------------------------------------------------------- */
	// Lights
	/* ------------------------------------------------------------------------------- */

private:
	void InitLights();
public:

	void TurnOffEveryLights();
	void TurnOnAndSetBrightnessRateOfLights(const FName& PageName, float BrightnessRate);
	void SetBrightnessRateOfLights(const FName& PageName, float BrightnessRate);
	
private:
	void TurnOnLights(const FName& PageName);

private:
	TMap<FName, TArray<ALight*>> LightsOfPages;


	// seungyoon.ji
	// HACK: 
	/* 
	* Cache the initial brightness of lights. It will be used in SetBrightnessRateOfLights(float).
	* Actually, The best option is that to make a custom light component, which has property to store the initial brightness of the light, And use that property instead of this container.
	* To do that, I have no choice but to replace all lights in the level, which already have set the light related properties.
	* I don't want to adjust lights settings again. So I use this container.
	*/
	TMap<ALight*, float> BrightnessOfLights;
	
};
