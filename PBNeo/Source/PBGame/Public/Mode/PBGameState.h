// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "PBTypes.h"
#include "PBGameState.generated.h"

/** ranked PlayerState map, created from the GameState */
typedef TMap<int32, TWeakObjectPtr<APBPlayerState> > RankedPlayerMap; 

UCLASS()
class APBGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

	virtual void Reset() override;
	virtual void UpdateServerTimeSeconds() override;

	/* ------------------------------------------------------------------------------- */
	// Game Mode
	/* ------------------------------------------------------------------------------- */

protected:

	// Variable to know which game mode is running now.
	UPROPERTY(Transient, Replicated)
	EPBGameModeType GameModeType;

public:

	void SetGameModeType(EPBGameModeType InGameModeType) { GameModeType = InGameModeType; }
	EPBGameModeType GetGameModeType() const { return GameModeType; }

	/* ------------------------------------------------------------------------------- */
	// Match Cycle
	/* ------------------------------------------------------------------------------- */

public:

	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentRound)
	int32 CurrentRound;

	UFUNCTION()
	void OnRep_CurrentRound();

public:
	void IncreaseRound();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerIncreaseRound();

protected:

	void InitMatchCycleSys();

	/* ------------------------------------------------------------------------------- */
	// Score per Team : Accumulated points across matches
	/* ------------------------------------------------------------------------------- */

public:

	void UpdateTeamTargetScore(uint32 Point);

	UFUNCTION(BlueprintCallable, Category = "PBGameState")
	int32 GetTeamTargetScore() const;

	UFUNCTION(BlueprintCallable, Category = "PBGameState")
	int32 GetTeamScore(EPBTeamType TeamType) const;

	UFUNCTION(BlueprintCallable, Category = "PBGameState")
	bool HasAnyAlivePlayer(EPBTeamType TeamType) const;

protected:

	void InitScorePerTeam();

	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScore;

	UPROPERTY(Transient, Replicated)
	int32 TargetTeamScore;

	void AddScoreToTeam(EPBTeamType TeamType, uint32 Point);

	/* ------------------------------------------------------------------------------- */
	// Missions per Team : Reset every match
	/* ------------------------------------------------------------------------------- */

public: // Methods

	void SetTeamMission(EPBTeamType Team, class APBMission* Mission);
	APBMission* GetTeamMission(EPBTeamType Team);

	void NotifyKill(APlayerState* Killer, APlayerState* Killed);
	void NotifyTrigger(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor);
	void NotifyGlobalEvent(FName EventName);
	void NotifyMatchTimeup();

	UFUNCTION(BlueprintCallable, Category = "PBGameState")
	int32 GetMatchPointOfTeam(EPBTeamType TeamType);

protected: // Implementations

	void OnMissionScoredPoint(APBMission* Mission, uint32 Point);
	void ResetMissionSys();

	UPROPERTY(Transient, Replicated)
	TArray<APBMission*> TeamMissions;

	/* ------------------------------------------------------------------------------- */
	// Getting Player Data
	/* ------------------------------------------------------------------------------- */

public:

	TArray<class APBPlayerState*> GetTeamPlayers(int32 TeamIndex) const;

	void GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap, bool bSortAsGreater = true) const;

	/* ------------------------------------------------------------------------------- */
	// Bomb Mission (추후 리팩토링 필요)
	/* ------------------------------------------------------------------------------- */
public:
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetBombRemainingTime(float InBombRemainingTime);

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BombRemainingTime)
	float BombRemainingTime;

	UFUNCTION()
	void OnRep_BombRemainingTime();

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};
