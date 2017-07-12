// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Mission/PBMission.h"
#include "PBMission_Escape_AI.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBMission_Escape_AI : public APBMission
{
	GENERATED_BODY()

	APBMission_Escape_AI();
	
	void NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor) override;

};
