// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineIdentityInterface.h"
#include "PBTypes.h"
#include "PBGameMode.generated.h"

class APBCharacter;
class APBAIController;
class APBPlayerState;
class APBPickup;
class FUniqueNetId;
struct FTakeDamageInfo;

enum class EPBPreMatchState : uint8
{
	None,
	Idle,
	WarmUp,
};

enum class EPBPostMatchState : uint8
{
	None,
	ConcludeMatch,
	ConcludeAllMatches,
};

UCLASS(config = Game)
class APBGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	APBGameMode();

	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;

	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitializeHUDForPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	//!+//@note: HandleStartingNewPlayer 호출시 Spectator가 되어야하는지 여기에서 결정합니다.
	bool HaveToBeSpectator();

	virtual void Tick(float DeltaSeconds) override;

	/* ------------------------------------------------------------------------------- */
	// Exit To Main Menu
	/* ------------------------------------------------------------------------------- */

public: 

	// [Called in server]
	void HandleReturnToMainMenu(bool UnexpectedFromClient);

	/* ------------------------------------------------------------------------------- */
	// Pausing
	/* ------------------------------------------------------------------------------- */

protected: // Implementation

	void DeterminePausability();

	/* ------------------------------------------------------------------------------- */
	// Match Cycle
	/* ------------------------------------------------------------------------------- */

	virtual bool ReadyToStartMatch_Implementation() override final;

	virtual bool ReadyToEndMatch_Implementation() override final;

	virtual void RestartGame() override;

public: // Public params

	UPROPERTY(config)
	int32 WarmupTime;

	UPROPERTY(config)
	int32 RoundTime;

	UPROPERTY(config)
	int32 TimeToConcludeMatch;

	UPROPERTY(config)
	int32 TimeToConcludeAllMatches;

protected: // Virtuals

virtual void PreConcludeMatch();
virtual  void PostConcludeMatch();

virtual bool MatchIsFinalMatch();
virtual void PreConcludeAllMatches();
virtual void PostConcludeAllMatches();

protected: // Implementations

	void InitMatchCycle();

	float GetTargetWarmupTime();

	void UpdateMatchCycleOnWaitingToStart();
	void UpdateMatchCycleOnPlayerLogIn();
	void UpdateMatchCycleOnStarted();
	void UpdateMatchCycleOnEnded();

	void PauseProgressMatchCycle();
	void ScheduleProgressMatchCycle(int32 DelaySecs);
	FTimerHandle Handle_CountdownToProgressMatchCycle;
	void CountdownToProgressMatchCycle();
	void ProgressMatchCycle();

protected: // State variables

	 // Sub-state when MatchState == WaitingToStart
	EPBPreMatchState PreMatchState;

	// Sub-state when MatchState == WaitingPostMatch
	EPBPostMatchState PostMatchState;

public:
	int32 GetCurrentRound() const;

protected: // Notifies

	DECLARE_EVENT(APBGameMode, FWarmUpStartedEvt)
	FWarmUpStartedEvt WarmUpStartedEvt;

	DECLARE_EVENT(APBGameMode, FWarmUpEndedEvt)
	FWarmUpEndedEvt WarmUpEndedEvt;

	DECLARE_EVENT(APBGameMode, FMatchTimeUpEvt)
	FMatchTimeUpEvt MatchTimeUpEvt;

	DECLARE_EVENT(APBGameMode, FPostRestartGameEvt)
	FPostRestartGameEvt PostRestartGameEvt;
	
	/* ------------------------------------------------------------------------------- */
	// Player Join Barrier
	/* ------------------------------------------------------------------------------- */

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/* ------------------------------------------------------------------------------- */
	// Warm up Broadcast
	/* ------------------------------------------------------------------------------- */
	 
protected: // Implementation

	void InitWarmUpBroadcastSys();
	void ReportWarmUpStarted();
	void ReportWarmUpEnded();
	void HandleWarmUpBroadcastOnLogin(APlayerController* NewPlayer);

	/* ------------------------------------------------------------------------------- */
	// Match Started Broadcast
	/* ------------------------------------------------------------------------------- */

protected: // Implementation

	void ReportMatchStarted();
	void HandleMatchStartBroadcastOnLogin(APlayerController* NewPlayer);

	/* ------------------------------------------------------------------------------- */
	// Death Broadcast Chain
	/* ------------------------------------------------------------------------------- */

public: // Public methods

	// [Called in server]
	virtual void Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType, AActor* DamageCauser, const FTakeDamageInfo& TakeDamageInfo);

protected: // Implementation

	void BroadcastDeath(APBPlayerState* KillerPlayerState, APBPlayerState* VictimPlayerState, const UDamageType* DamageType, const FTakeDamageInfo& TakeDamageInfo);

protected: // Notifies

	DECLARE_EVENT_TwoParams(APBGameMode, FOnKilledEvt, AController* /* Killer */, AController* /* KilledPlayer */)
	FOnKilledEvt OnKilledEvt;

	/* ------------------------------------------------------------------------------- */
	//  Player Score
	/* ------------------------------------------------------------------------------- */

public: // Public params

	UPROPERTY(config)
	int32 PlayerKillScore;

	UPROPERTY(config)
	int32 PlayerDeathScore;

protected: // Implementation

	void InitPlayerScoreSys();
	void UpdatePlayerScores(AController* Killer, AController* KilledPlayer);

	/* ------------------------------------------------------------------------------- */
	// Teams
	/* ------------------------------------------------------------------------------- */

public:

	static FString GetTeamOptionName();

protected: // Virtuals

	virtual EPBTeamType ChooseTeam(class APBPlayerState* ForPlayerState) const;

protected: // Implementation

	void InitTeamSys();
	void ChooseTeamUponLogin(APlayerController* NewPlayer, const FString& Options);

	/* ------------------------------------------------------------------------------- */
	//  Team Score
	/* ------------------------------------------------------------------------------- */

protected:

	void TeamScoreHandleInitGame(const FString& Options);
	virtual int32 GetTargetTeamScore(const FString& Options);

	// Only used in server
	int32 TargetScoreCache;

	void TeamScoreHandleBeginPlay();

	/* ------------------------------------------------------------------------------- */
	//  Missions and Win/Loss for Teams
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = Trigger)
	virtual void OnTriggerEvent(UPrimitiveComponent* TriggerComp, AActor* TriggeredActor);

	UFUNCTION(BlueprintCallable, Category = Trigger)
	virtual void OnGlobalEvent(FName EventName);

protected: // Virtuals

	virtual class APBMission* CreateMissionForTeam(EPBTeamType Team);

protected: // Implementation

	void InitMissionSys();
	void DecideMissionsBeforeWarmUp();
	void NotifyMissionsOfKill(AController* Killer, AController* Killed);
	void NotifyMissionOfTimeUp();
	void HandleEarlyMatchConclusion();
	void ConcludeMissionAfterMatch();

	/* ------------------------------------------------------------------------------- */
	// Respawn
	/* ------------------------------------------------------------------------------- */

public:
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void RestartPlayer(class AController* NewPlayer) override;

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void BroadcastSpawn(class AController* NewPlayer);

public: // Public params

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Unbeatable)
	float UnbeatableTimeUponSpawn;

protected: // Virtuals

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player);
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player);

protected: // Implementations

	void InitSpawnSys();

	/* ------------------------------------------------------------------------------- */
	// Damage Modification
	/* ------------------------------------------------------------------------------- */

public: // Public params

	UPROPERTY(config)
	float DamageSelfScale;

public: // Public methods

	virtual bool CanDealDamage(APBPlayerState* DamageInstigator, APBPlayerState* DamagedPlayer) const;
	float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

protected: // Implementation

	void InitDamageModSys();

	/* ------------------------------------------------------------------------------- */
	// Mode Special Weapon
	/* ------------------------------------------------------------------------------- */

public: // Public methods

	virtual void AddSpecifyWeaponsTo(APBCharacter* Pawn);

	/* ------------------------------------------------------------------------------- */
	// HUD
	/* ------------------------------------------------------------------------------- */

public: // Public params

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes, meta = (DisplayName = "InGameWidget"))
	TSubclassOf<class UPBInGameWidget> InGameWidgetClass;

protected: // Implementation

	void ReportInGameWidgetClasses(APlayerController* NewPlayer);

	/* ------------------------------------------------------------------------------- */
	// Killmark
	/* ------------------------------------------------------------------------------- */

public: // Implementation

	virtual void UpdateKillMark(AController* Killer, const TArray<AController*>& KilledPlayers, AActor* DamageCauser, const TArray<struct FTakeDamageInfo>& TakeDamageInfos);

	/* ------------------------------------------------------------------------------- */
	// Pickups
	/* ------------------------------------------------------------------------------- */

public: // Public variable

	// If the delay=0 then don't destroy
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float PickupDestroyDelay;

	UPROPERTY()
	TArray<APBPickup*> LevelPickups;

	class APBPickup* NewPickupItem(TSubclassOf<APBPickup> WantsType, const FTransform& SpawnTransform);
	class APBPickup* NewPickupWeap(class APBWeapon* InWeap, const FTransform& SpawnTransform);
protected:

	TMultiMap<UClass*, APBPickup*> PickupItemMap;

	UPROPERTY()
	TSubclassOf<class APBPickupWeapon> PickupWeapBaseClass;

	void InitPickup();
protected:

	/* ------------------------------------------------------------------------------- */
	// Bots
	/* ------------------------------------------------------------------------------- */

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

public: // Public params

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APawn> BotPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APBAIController> BotControllerClass;

public: // Public method

	static FString GetBotsCountOptionName();

	APBAIController* CreateBot(int32 BotNum);

protected: // Implementation

	void InitBotSys();

	void BotSysHandleInitGame(const FString& Options);

	UPROPERTY(config)
	int32 TargetBotNum;

	void BotSysHandleWarmUp();

	virtual void InitBot(APBAIController* AIC, int32 BotNum);

	void BotSysHandleMatchStart();

	void SpawnBotAtStartTransform(AController* BotController);

	/* ------------------------------------------------------------------------------- */
	// Cheats
	/* ------------------------------------------------------------------------------- */

	virtual bool AllowCheats(APlayerController* P) override;

	/* ------------------------------------------------------------------------------- */
	// Table
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditDefaultsOnly, Category = GameModeData)
	int32 ItemID;
	void SetItemID(int32 ID) { ItemID = ID; }
	int32 GetItemID() { return ItemID; }
	virtual bool ApplyTableData(const struct FPBGameModeTableData* Data);

	UPROPERTY(EditDefaultsOnly, Category = Config, meta = (DisplayName = "UseTableData (Need ItemID)"))
	bool UseTableDataToConfig;
	bool GetUseTableTata() { return UseTableDataToConfig; }
	FPBGameModeTableData* GetTableData();

protected:

	void InitTableSys();
	class UPBGameTableManager* GetGameTableManager() const;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */

};
