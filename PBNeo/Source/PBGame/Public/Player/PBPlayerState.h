// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "PBTypes.h"
#include "FPBKillMarkProperties.h"
#include "PBKillMarkTypes.h"
#include "PBPlayerState.generated.h"

/*
USTRUCT()
struct FPlayerDebugContext
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = PlayerDebug)
	FString MatchState;

	UPROPERTY(EditDefaultsOnly, Category = PlayerDebug)
	int32 CharacterCount;

	UPROPERTY(EditDefaultsOnly, Category = PlayerDebug)
	bool bPossessed;
};
*/

UCLASS()
class APBPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	virtual void Reset() override;

	virtual void CopyProperties(class APlayerState* PlayerState) override;

	virtual void OverrideWith(APlayerState* PlayerState);

	/* ------------------------------------------------------------------------------- */
	// Fixes
	/* ------------------------------------------------------------------------------- */

	virtual void UnregisterPlayerWithSession() override;

	/* ------------------------------------------------------------------------------- */
	// Utilities
	/* ------------------------------------------------------------------------------- */

public:

	APBCharacter* GetOwnerCharacter();

	// You can use this to get remote player's character
	UPROPERTY(BlueprintReadOnly, Transient, Replicated)
	APBCharacter* OwnerCharacter;

	/* ------------------------------------------------------------------------------- */
	// PlayerName
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, category = "PBPlayerState")
	FString GetShortPlayerName() const;

	/* ------------------------------------------------------------------------------- */
	// Quitter
	/* ------------------------------------------------------------------------------- */

public:
	void SetQuitter(bool bInQuitter);
	bool IsQuitter() const;

protected:
	void InitQuitter();
	void ResetQuitter();

protected:
	UPROPERTY()
	uint8 bQuitter : 1;

	/* ------------------------------------------------------------------------------- */
	// Death Broadcast Chain
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class APBPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class APBPlayerState* KilledPlayerState, const FTakeDamageInfo& TakeDamageInfo);


	UFUNCTION(Reliable, NetMulticast)
	void BroadcastSpawn();

	/* ------------------------------------------------------------------------------- */
	// Kill/Death/Score
	/* ------------------------------------------------------------------------------- */

public:	// [Call on server]
	void ScoreKill(APBPlayerState* Victim, int32 Points);
	void ScoreDeath(APBPlayerState* KilledBy, int32 Points);

	// Valid only on server
	bool DiedAtLeastOnceInMatch();

	// Call anywhere
	UFUNCTION(BlueprintCallable, Category = "PBPlayerState")
	int32 GetKills() const;

	UFUNCTION(BlueprintCallable, Category = "PBPlayerState")
	int32 GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "PBPlayerState")
	float GetScore() const;

	/** This Value is used to sort squad info. */
	int32 GetSquadSortValue() const;

protected:
	void InitKillDeathScore();
	void ResetKillDeathScore();
	void CopyKillDeathScore(APlayerState* NewPlayerState);

protected:
	// Valid only on server
	bool bDiedAtLeastOnceInMatch;

	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/* ------------------------------------------------------------------------------- */
	// Kill Mark Properties
	/* ------------------------------------------------------------------------------- */

protected:
	UPROPERTY(Transient, Replicated)
	FPBKillMarkProperties KillMarkProperties;
	
public:
	FPBKillMarkProperties& GetKillMarkProperties();

	// [Server call only]
	void UpdateKillMark(AController* Killer, const TArray<AController*>& KilledPlayers, AActor* DamageCauser, const TArray<FTakeDamageInfo>& TakeDamageInfos);

private:
	// When character die, some properties should be reset.
	void ResetKillMarkProperties();

	//TArray<int32> ConvertKillMarkTypesToInteger(const TArray<FKillMarkType>& CheckedKillMarks);

	//void UpdateKillMarkBeforeChecking(APBPlayerState* VictimPlayerState, EWeaponType WeapType, bool bHeadShot = false);
	//void UpdateKillMarkAfterChecking(const TArray<EKillMarkTypes>& CheckedKillMarks, APBPlayerState* VictimPlayerState, EWeaponType WeapType);

	void UpdateKillMarkBeforeChecking(APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo);
	void UpdateKillMarkAfterChecking(const TArray<int32>& CheckedKillMarks, APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo);

protected:
	// [RPC] Notify the kill mark events to client
	UFUNCTION(Reliable, Client)
	void ClientKillMarkEvent(const TArray<int32>& CheckedKillMarks, EWeaponType WeapType);

	/* ------------------------------------------------------------------------------- */
	// Team Type
	/* ------------------------------------------------------------------------------- */

public:
	// [Call on Server]
	void SetTeamType(EPBTeamType NewTeamType);

	// [Call anywhere]
	UFUNCTION(BlueprintCallable, Category = "PBPlayerState")
	EPBTeamType GetTeamType() const;

	int32 GetTeamSlotIdx() { return TeamSlotIdx; }

protected:
	void InitTeamType();

	void CopyTeamType(APlayerState* NewPlayerState);

	void CopyTeamTypeFrom(APlayerState* OldPlayerState);

	UFUNCTION()
	void OnRep_TeamType();

	void UpdateTeamColors();

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamType)
	EPBTeamType TeamType;	

	int32 TeamSlotIdx;

	/* ------------------------------------------------------------------------------- */
	// Mission
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(BlueprintCallable, Category = "Mission")
	APBMission* GetOwnMission();

	/* ------------------------------------------------------------------------------- */
	// Data Sync
	/* ------------------------------------------------------------------------------- */

public:

	bool IsInitiallyReplicated();
	void SetInitiallyReplicated();

	bool IsPlayerStarted();
	void SetPlayerStarted();

	bool HasCharacterItems();	
	bool HasWeaponItems();

	UPROPERTY(Transient, Replicated)
	TArray<int32> ReplicateEquippedCharItems;	// Character Item ID list of Teams
	UPROPERTY(Transient, Replicated)
	TArray<int32> ReplicateEquippedWeapItems;	// Weapon slot Item ID list

protected:

	// Valid everywhere
	bool bIsInitiallyReplicated;

	// Valid only on server
	bool bIsPlayerStarted;

public:
	//UPROPERTY(Transient, Replicated)
	//FPlayerDebugContext PlayerDebugContext;
	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};
