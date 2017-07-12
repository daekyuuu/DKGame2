// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBGameState.h"
#include "GameFramework/Actor.h"
#include "PBMission.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class PBGAME_API APBMission : public AActor
{
	GENERATED_BODY()

public:

	APBMission();

	// Each Mission is managed by GameState and associated with a EPBTeamType
	void Init(EPBTeamType OwningTeam);
	APBGameState* GetOwningGameState();
	UPROPERTY(Transient, BlueprintReadOnly, Replicated, Category = Team)
	EPBTeamType OwningTeam;

	virtual void NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed);

	virtual void NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor);

	virtual void NotifyGlobalEvent(FName EventName);

	virtual void NotifyMatchTimeup();

	//  The team with most match point at match-end wins
	UPROPERTY(Transient, BlueprintReadOnly, Replicated, Category = MatchPoint)
	int32 CurrentMatchPoint;

	// Call when you want to give the owning team score (score is accumulated across matches)
	DECLARE_EVENT_TwoParams(APBMission, FTeamScoredEvt, APBMission* /*Self*/, uint32 /*Score*/)
	FTeamScoredEvt TeamScoredEvt;

	// If this is called by a mission, the game mode will immediately start concluding winner acc to match points
	DECLARE_EVENT(APBMission, FWantToEndMatchEvt)
	FWantToEndMatchEvt WantToEndMatchEvt;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Constant parameter that differs acc to missions
	bool bPlayerCanRespawn;
};
