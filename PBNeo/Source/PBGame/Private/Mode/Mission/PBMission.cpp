// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMission.h"
#include "PBPlayerState.h"

APBMission::APBMission()
{
	OwningTeam = EPBTeamType::Max;
	CurrentMatchPoint = 0;
	bPlayerCanRespawn = true;

	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
}

void APBMission::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APBMission, OwningTeam);
	DOREPLIFETIME(APBMission, CurrentMatchPoint);
}

void APBMission::Init(EPBTeamType InOwningTeam)
{
	OwningTeam = InOwningTeam;
}

APBGameState* APBMission::GetOwningGameState()
{
	if (GetWorld())
	{
		return Cast<APBGameState>(GetWorld()->GetGameState());
	}

	return nullptr;
}

void APBMission::NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed)
{
}

void APBMission::NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor)
{
}

void APBMission::NotifyGlobalEvent(FName EventName)
{

}

void APBMission::NotifyMatchTimeup()
{
}

void APBMission::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WantToEndMatchEvt.Clear();
	TeamScoredEvt.Clear();
}
