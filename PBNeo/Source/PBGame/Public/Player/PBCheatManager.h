// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBCheatManager.generated.h"

UCLASS()
class UPBCheatManager : public UCheatManager
{
	GENERATED_UCLASS_BODY()

public: // server cheat functions
	UFUNCTION(exec)
	void CheatInfiniteAmmo();

	UFUNCTION(exec)
	void CheatInfiniteClip();

	UFUNCTION(exec)
	void CheatGodMode();

	UFUNCTION(exec)
	void CheatHealthRegen();

	UFUNCTION(exec)
	void CheatSuicide();

	UFUNCTION(exec)
	void CheatUnbeatable();

	UFUNCTION(exec)
	void CheatSetHealth(int32 Value = 99999);

	UFUNCTION(exec)
	void CheatPauseMatch();

	UFUNCTION(exec)
	void CheatSetMatchTime(int32 Value);

	UFUNCTION(exec)
	void CheatForceMatchStart();

	UFUNCTION(exec)
	void CheatSetTeam(EPBTeamType NewTeamType);

	UFUNCTION(exec)
	void CheatSpawnBot();

	UFUNCTION(exec)
	void SyncTableToCharacterBP();
	UFUNCTION(exec)
	void SyncTableToWeaponBP();
	UFUNCTION(exec)
	void SyncTableToGameModeBP();
	UFUNCTION(exec)
	void SyncTableToAllBP();

	UFUNCTION(exec)
	void CheatWeapon(int32 ItemID=0);

	UFUNCTION(exec)
	void CheatWeaponItemList();

	UFUNCTION(exec)
	void RestartGame(FString MapPathName="/Game/Maps/TestLevel_A", bool bAbsolute = false);

	UFUNCTION(exec)
	void LoadMap(FString MapPathName);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LoadMapComplete();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void UnloadMap();

	UFUNCTION(exec)
	void PlayAnim(FString MontagePathName ="/Game/PBCharacterAnimations/");

	/** Weapon Pierce Debug Flag On/Off **/
	UFUNCTION(exec)
	void DebugPierce();

	//추락 데미지 관련 정보만 출력합니다.
	UFUNCTION(exec)
	void DebugFallOffDamage();

	//추락 상태 관련 정보만 출력합니다.
	UFUNCTION(exec)
	void DebugFallOffStatus();

	UFUNCTION(exec)
	void DebugAddRoomSlots(int32 MaxSlotNum = 8);

private: // Helpers
	class APBPlayerController* GetPBPlayerController() const;
	class APBCharacter* GetPBCharacter() const;
	void ServerCheat(const FString& Msg);

public:
	/** Weapon Pierce Debug Log Visible Toggle **/
	static bool DebugPierceToggle;

	static bool EnabledDebugFallOffDamage;

	static bool EnabledDebugFallOffStatus;
};
