// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMission_Escape_AI.h"

APBMission_Escape_AI::APBMission_Escape_AI()
{
}

void APBMission_Escape_AI::NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor)
{
	if (TriggerComp && TriggerComp->ComponentHasTag(FName(TEXT("EscapeGate"))))
	{
		APBCharacter* Pawn = Cast<APBCharacter>(TriggeredActor);
		if (Pawn && !Pawn->IsBot() && Pawn->IsAlive())
		{
			CurrentMatchPoint += 1;
			TeamScoredEvt.Broadcast(this, 1);
			WantToEndMatchEvt.Broadcast();
		}
	}
}