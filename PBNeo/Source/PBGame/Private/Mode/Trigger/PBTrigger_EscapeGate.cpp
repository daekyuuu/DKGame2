// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBTrigger_EscapeGate.h"
#include "Mode/PBGameMode.h"

void APBTrigger_EscapeGate::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		APBGameMode* GameMode = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnTriggerEvent(OverlappedComponent, OtherActor);
		}		
	}
}
