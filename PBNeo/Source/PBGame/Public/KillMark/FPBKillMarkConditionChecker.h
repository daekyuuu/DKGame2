// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBKillMarkTypes.h"
#include "PBKillMarkUtility.h"
/**
 * 
 */

class FPBKillMarkConditionChecker
{
public:
	struct Type
	{
		/*
			EKillMarkCategory::Rare
		*/
		PBKillMarkType(SpecialGunner, EKillMarkCategory::Rare);
		PBKillMarkType(Assassin, EKillMarkCategory::Rare);
		PBKillMarkType(BombShot, EKillMarkCategory::Rare);
		PBKillMarkType(OneShotOneKill, EKillMarkCategory::Rare);
		
		/*
		EKillMarkCategory::Special
		*/
		PBKillMarkType(ChainSlugger, EKillMarkCategory::Special);
		PBKillMarkType(PiercingShot, EKillMarkCategory::Special);
		PBKillMarkType(MassKill, EKillMarkCategory::Special);
		PBKillMarkType(ChainStopper, EKillMarkCategory::Special);
		PBKillMarkType(ChainKiller, EKillMarkCategory::Special);
		PBKillMarkType(ChainHeadShot, EKillMarkCategory::Special);
		PBKillMarkType(HeadShot, EKillMarkCategory::Special);
		
		/*
		EKillMarkCategory::Revenge
		*/
		PBKillMarkType(HotKiller, EKillMarkCategory::Revenge);
		PBKillMarkType(Nightmare, EKillMarkCategory::Revenge);
		PBKillMarkType(MeatChopper, EKillMarkCategory::Revenge);
		PBKillMarkType(Payback, EKillMarkCategory::Revenge);
		PBKillMarkType(Avenger, EKillMarkCategory::Revenge);
		PBKillMarkType(Stalker, EKillMarkCategory::Revenge);
		PBKillMarkType(LoveShot, EKillMarkCategory::Revenge);

		/*
		EKillMarkCategory::Normal
		*/
		PBKillMarkType(TripleKill, EKillMarkCategory::Normal);
		PBKillMarkType(DoubleKill, EKillMarkCategory::Normal);
		PBKillMarkType(FallOff, EKillMarkCategory::Normal);

	private: 
		Type() {}
	};

public:
	// this function doesn't include MassKill and PiercingShot
	static TArray<KillMarkType> CheckSingleKill(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo);
	static TArray<KillMarkType> CheckKillOwnself(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo);
	// this function include all of kill mark checking logic
	static TArray<KillMarkType> CheckAll(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo);
	
	/**
	* @return (killer's KillCount - Victim's killCount)
	*/
	static EWeaponType GetWeaponTypeFromDamagCauser(AActor *DamageCauser);
	static int32 GetKillGap(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState);
};
typedef FPBKillMarkConditionChecker::Type FPBKillMarkConditionType;