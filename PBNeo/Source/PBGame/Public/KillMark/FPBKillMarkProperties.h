// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FPBKillMarkProperties.generated.h"

/**
 *
 */

USTRUCT(BlueprintType)
struct FHotKillerData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	class APBPlayerState* TargetPlayerState;

	UPROPERTY()
	int32 NumKills;

	// I've got hot killer of this target
	UPROPERTY()
	int32 NumHotKillerGet;

	// I've payback to this target without a death
	UPROPERTY()
	int32 NumSerialPayback;

public:

	FHotKillerData();
	void ClearAllCount();
};

USTRUCT(BlueprintType)
struct FPBKillMarkProperties
{
	GENERATED_USTRUCT_BODY()


	//////////////////////////////////////////////////////////////////////////
	/// Cleared if die

	UPROPERTY()
	int32 NumSerialKills;

	UPROPERTY()
	int32 NumMeleeKills;

	UPROPERTY()
	int32 NumHeadKills;

	UPROPERTY()
	bool bAchieveOneShotOnekill;

	//////////////////////////////////////////////////////////////////////////
	/// Cleared when character is possessed

	UPROPERTY()
	APBPlayerState* PlayerWhoKilledMe;

	//////////////////////////////////////////////////////////////////////////
	/// cleared only game started

	UPROPERTY()
	TArray<FHotKillerData> HotKillerDatas;

	//////////////////////////////////////////////////////////////////////////
	/// cleared when weapon firing

	// to check one shot one kill
	UPROPERTY()
	int32 FiringCnt;

public:
	FPBKillMarkProperties();

	bool FindIndexOfHotKillerData(class APBPlayerState* PlayerState, int32& outIndex);

	// kills about specific user
	int32 IncreaseUserKill(class APBPlayerState* VictimPlayerState);
	int32 IncreaseHotKiller(class APBPlayerState* VictimPlayerState);
	void ClearHotKillerData(class APBPlayerState* VictimPlayerState);

	int32 IncreaseSerialPayback(class APBPlayerState* VictimPlayerState);


	const FHotKillerData* GetHotkillerData(class APBPlayerState* PlayerState);
	void ResetRespawnClearProperties();
	void Init();
	void ResetAll();
};
