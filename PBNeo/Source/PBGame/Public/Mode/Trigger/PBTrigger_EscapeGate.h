// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Trigger/PBBoxTrigger.h"
#include "PBTrigger_EscapeGate.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBTrigger_EscapeGate : public APBBoxTrigger
{

	GENERATED_BODY()
	
	void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult) override;
	
};
