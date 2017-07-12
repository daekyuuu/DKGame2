// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Online/PBUserInfo.h"
#include "PBCharacterScene.generated.h"

class APBCharacter;
struct FPBUserBaseInfo;

struct FCharacterSceneSpawnInfo
{	
	APBCharacter* DefaultPawn;
	FPBUserBaseInfo* UserBaseInfoPtr;

	FCharacterSceneSpawnInfo()
	{
		DefaultPawn = nullptr;
		UserBaseInfoPtr = nullptr;
	}
};

UCLASS()
class PBGAME_API UPBCharacterScene : public UObject
{
	GENERATED_BODY()
	
public:	
	
	void Init(class APlayerController* InOwningPlayer);

	void Reset();

	virtual UWorld* GetWorld() const override
	{
		return OwningPlayer ? OwningPlayer->GetWorld() : nullptr;
	}

	void DestroySpawnedCharacters();
	void PendingSpawnTeamCharacters(float SpawnInterval = 0.3f, bool bPlayStartAnimOnLoaded=false, bool bVisibleOnLoaded = false);
	void FlushSpawnTeamCharacters(bool bPlayStartAnimOnLoaded = false, bool bVisibleOnLoaded = true);

	UPROPERTY()
	TMap<int32, FTimerHandle> PendingSpawnTimerHandles;

protected:
	void BindEvents();
	void NotifyCharacterIsSelected(int32 Id);
	void NotifyWeapIsSelected(int32 Id);	
	
protected:
	FCharacterSceneSpawnInfo* FindCachedRoomSpawnInfo(EPBTeamType TeamType, int32 SlotIdx);
	FCharacterSceneSpawnInfo* FindCachedLocalSpawnInfo(EPBTeamType TeamType, int32 SlotIdx);
	APBCharacter* FindLocalCharacter(EPBTeamType TeamType, int32 SlotIdx);

	UFUNCTION()
	void SpawnLocalUserCharacter(class APlayerController* InOwningPlayer);

	UFUNCTION()
	void SpawnRoomUserCharacter(FPBRoomSlotInfo& RoomSlotInfo, bool bPlayStartAnimOnLoaded = false, bool bVisibleOnLoaded = true);

	UFUNCTION()
	void SpawnRoomUserCharacterWithDelay(FPBRoomSlotInfo& RoomSlotInfo, float Delay, bool bPlayStartAnimOnLoaded = false, bool bVisibleOnLoaded = true);

	UFUNCTION()
	void SpawnRoomUserCharacterByTeamSlot(EPBTeamType TeamType, int32 SlotIdx, bool bPlayStartAnimOnLoaded = false, bool bVisibleOnLoaded = true);
	
private:
	class APawn* SpawnTeamCharacter(int32 CharItemID, bool bLocalPlayer, EPBTeamType TeamType, int32 TeamSlotIdx);
	class APBCharacter* GetCharacterCDO(int32 Id);
	void SetCharacter(class APawn* TargetPawn, int32 CharItemID);
	void EquipWeapon(class APawn* TargetPawn, int32 WeapItemID);

	void DetermineDefaultPawnClass(int32 CharItemID, bool bLocalPlayer);
	int32 DetermineStartSpotIdx(bool bLocalPlayer);
	class APawn* SpawnDefaultPawnAtStartSpot(EPBTeamType SpotTeamType, int32 SpotIdx);
	class AActor* FindTeamStartSpot(EPBTeamType SpotTeamType, int32 SpotIdx);

private:
	UPROPERTY()
	class APlayerController* OwningPlayer;

	TMap<EPBTeamType, TMap<int32, FCharacterSceneSpawnInfo>> CachedRoomUserSpawnInfoMap;
	TMap<EPBTeamType, TMap<int32, FCharacterSceneSpawnInfo>> CachedLocalUserSpawnInfoMap;

	int32 LastSpawnSpotIdx;

};
