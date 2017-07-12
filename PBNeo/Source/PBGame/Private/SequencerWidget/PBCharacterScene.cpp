// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCharacterScene.h"
#include "PBMVCharacter.h"

#include "PBGameTableManager.h"
#include "PBItemTable_Char.h"
#include "PBTableManager.h"
#include "PBItemTableManager.h"

#include "Player/PBCharacter.h"
#include "Player/PBPlayerController_Menu.h"
#include "Player/PBLocalPlayer.h"
#include "Weapons/PBWeapon.h"
#include "PBGameInstance.h"
#include "PBLobby.h"
#include "PBLoadoutPage.h"
#include "Mode/PBMode_LobbyMenu.h"
#include "Online/PBRoomUserInfoManager.h"

void UPBCharacterScene::Init(class APlayerController* InOwningPlayer)
{
	Reset();

	OwningPlayer = InOwningPlayer;

	if (nullptr == OwningPlayer)
	{
		return;
	}

	SpawnLocalUserCharacter(InOwningPlayer);

	BindEvents();	
}

void UPBCharacterScene::Reset()
{
	OwningPlayer = nullptr;

	DestroySpawnedCharacters();

	PendingSpawnTimerHandles.Empty();

	LastSpawnSpotIdx = 1; //1 ~ 8
}

void UPBCharacterScene::DestroySpawnedCharacters()
{
	// Other Player
	for (auto& TeamSpawnInfo : CachedRoomUserSpawnInfoMap)
	{
		for (auto& SpawnInfoMap : TeamSpawnInfo.Value)
		{
			if (SpawnInfoMap.Value.DefaultPawn)
			{
				SpawnInfoMap.Value.DefaultPawn->Destroy();
			}
		}
	}
	CachedRoomUserSpawnInfoMap.Empty();

	// Local Player
	for (auto& TeamSpawnInfo : CachedLocalUserSpawnInfoMap)
	{
		for (auto& SpawnInfoMap : TeamSpawnInfo.Value)
		{
			if (SpawnInfoMap.Value.DefaultPawn)
			{
				SpawnInfoMap.Value.DefaultPawn->Destroy();
			}
		}
	}
	CachedLocalUserSpawnInfoMap.Empty();
}

void UPBCharacterScene::BindEvents()
{
	auto GI = Cast<UPBGameInstance>(UPBGameplayStatics::GetGameInstance(OwningPlayer));
	if (GI)
	{
		auto Lobby = GI->LobbyInstance;
		if (Lobby)
		{
			UPBLoadoutPage* LoadoutPage = Cast<UPBLoadoutPage>(Lobby->GetPageInstance(EPageType::Loadout));
			if (LoadoutPage)
			{
				LoadoutPage->SoldierSelectEvt.AddUObject(this, &UPBCharacterScene::NotifyCharacterIsSelected);
				LoadoutPage->WeapSelectEvt.AddUObject(this, &UPBCharacterScene::NotifyWeapIsSelected);
			}
		}

	}
}

void UPBCharacterScene::NotifyCharacterIsSelected(int32 Id)
{
	EPBTeamType TeamType = UPBGameplayStatics::GetTeamTypeFromCharId(OwningPlayer, Id);
	SetCharacter(FindLocalCharacter(TeamType, 0), Id);
}

void UPBCharacterScene::NotifyWeapIsSelected(int32 Id)
{
	EquipWeapon(FindLocalCharacter(EPBTeamType::Alpha, 0), Id);
	EquipWeapon(FindLocalCharacter(EPBTeamType::Bravo, 0), Id);
}

APBCharacter* UPBCharacterScene::FindLocalCharacter(EPBTeamType TeamType, int32 SlotIdx)
{
	FCharacterSceneSpawnInfo* SpawnInfo = FindCachedLocalSpawnInfo(TeamType, SlotIdx);
	return SpawnInfo ? SpawnInfo->DefaultPawn : nullptr;
}

FCharacterSceneSpawnInfo* UPBCharacterScene::FindCachedLocalSpawnInfo(EPBTeamType TeamType, int32 SlotIdx)
{
	TMap<int32, FCharacterSceneSpawnInfo>* TeamSpawnInfoMap = CachedLocalUserSpawnInfoMap.Find(TeamType);
	return TeamSpawnInfoMap ? TeamSpawnInfoMap->Find(SlotIdx) : nullptr;
}

FCharacterSceneSpawnInfo* UPBCharacterScene::FindCachedRoomSpawnInfo(EPBTeamType TeamType, int32 SlotIdx)
{
	TMap<int32, FCharacterSceneSpawnInfo>* TeamSpawnInfoMap = CachedRoomUserSpawnInfoMap.Find(TeamType);
	return TeamSpawnInfoMap ? TeamSpawnInfoMap->Find(SlotIdx) : nullptr;
}

void UPBCharacterScene::SpawnLocalUserCharacter(class APlayerController* InOwningPlayer)
{
	UPBPersistentUser* Persitent = UPBGameplayStatics::GetPersistentUserInfo(InOwningPlayer);

	if (Persitent)
	{
		const int32 FirstTeamSlotIdx = 0;
		APBMVCharacter* Pawn = nullptr;
		int32 CharItemID = Persitent->GetBaseUserInfo().GetCharItemID(EPBTeamType::Alpha);
		int32 WeapItemID = Persitent->GetBaseUserInfo().GetWeapItemID(EWeaponSlot::Primary);

		Pawn = Cast<APBMVCharacter>(SpawnTeamCharacter(CharItemID, true, EPBTeamType::Alpha, FirstTeamSlotIdx));
		if (Pawn)
		{
			SetCharacter(Pawn, CharItemID);
			EquipWeapon(Pawn, WeapItemID);

			FCharacterSceneSpawnInfo SpawnInfoA;
			SpawnInfoA.DefaultPawn = Pawn;
			SpawnInfoA.UserBaseInfoPtr = &Persitent->GetBaseUserInfo();
			CachedLocalUserSpawnInfoMap.FindOrAdd(EPBTeamType::Alpha).FindOrAdd(FirstTeamSlotIdx) = SpawnInfoA;
		}

		CharItemID = Persitent->GetBaseUserInfo().GetCharItemID(EPBTeamType::Bravo);
		Pawn = Cast<APBMVCharacter>(SpawnTeamCharacter(CharItemID, true, EPBTeamType::Bravo, FirstTeamSlotIdx));
		if (Pawn)
		{
			SetCharacter(Pawn, CharItemID);
			EquipWeapon(Pawn, WeapItemID);

			FCharacterSceneSpawnInfo SpawnInfoB;
			SpawnInfoB.DefaultPawn = Pawn;
			SpawnInfoB.UserBaseInfoPtr = &Persitent->GetBaseUserInfo();
			CachedLocalUserSpawnInfoMap.FindOrAdd(EPBTeamType::Bravo).FindOrAdd(FirstTeamSlotIdx) = SpawnInfoB;
		}
	}
}

void UPBCharacterScene::FlushSpawnTeamCharacters(bool bPlayStartAnimOnLoaded, bool bVisibleOnLoaded)
{
	PendingSpawnTeamCharacters(0.f, bPlayStartAnimOnLoaded, bVisibleOnLoaded);
}

void UPBCharacterScene::PendingSpawnTeamCharacters(float SpawnInterval, bool bPlayStartAnimOnLoaded, bool bVisibleOnLoaded)
{
	auto UserManager = UPBRoomUserInfoManager::Get(OwningPlayer);
	if (UserManager == nullptr)
		return;

	FPBRoomSlotInfo& MySlotInfo = UserManager->GetMyUserSlotInfo();
	EPBTeamType MyTeamType = MySlotInfo.IsValidBase() ? MySlotInfo.TeamType : EPBTeamType::Alpha;
	float DelayPendingSpawnTime = 0.f;

	// 예약 되어있는 스폰 핸들러를 모두 삭제한다.
	UWorld* World = GetWorld();
	if (World)
	{
		for (auto PendingTimerHandle : PendingSpawnTimerHandles)
		{
			World->GetTimerManager().ClearTimer(PendingTimerHandle.Value);
		}
		PendingSpawnTimerHandles.Empty();
	}

	for (auto& TeamSlots : UserManager->GetUserTeamSlots(MyTeamType))
	{
		FPBRoomSlotInfo& SlotInfo = TeamSlots.Value;
		if (SlotInfo.IsValidBase() && !SlotInfo.bMine && !SlotInfo.bSpectator)
		{
			if (SpawnInterval <= 0.f)
			{
				SpawnRoomUserCharacter(SlotInfo, bPlayStartAnimOnLoaded, bVisibleOnLoaded);
			}
			else
			{
				SpawnRoomUserCharacterWithDelay(SlotInfo, DelayPendingSpawnTime, bPlayStartAnimOnLoaded, bVisibleOnLoaded);
				DelayPendingSpawnTime += SpawnInterval;
			}
		}
	}
}

void UPBCharacterScene::SpawnRoomUserCharacterWithDelay(FPBRoomSlotInfo& RoomSlotInfo, float Delay, bool bPlayStartAnimOnLoaded, bool bVisibleOnLoaded)
{	
	// 이미 타이머가 등록 되어있으면 삭제
	UWorld* World = GetWorld();
	FTimerHandle* CachedTimerHandle = PendingSpawnTimerHandles.Find(RoomSlotInfo.TeamSlotIdx);
	if (World && CachedTimerHandle)
	{
		World->GetTimerManager().ClearTimer(*CachedTimerHandle);
	}

	// 타이머 새로 등록함
	FTimerHandle SpawnTimerHandle;
	PendingSpawnTimerHandles.FindOrAdd(RoomSlotInfo.TeamSlotIdx) = SpawnTimerHandle;

	FTimerDelegate SpawnTimerDelegate;
	SpawnTimerDelegate.BindUFunction(this, FName("SpawnRoomUserCharacterByTeamSlot"), RoomSlotInfo.TeamType, RoomSlotInfo.TeamSlotIdx, bPlayStartAnimOnLoaded, bVisibleOnLoaded);
		
	if (World)
	{
		World->GetTimerManager().SetTimer(PendingSpawnTimerHandles[RoomSlotInfo.TeamSlotIdx], SpawnTimerDelegate, Delay, false);
	}
}

void UPBCharacterScene::SpawnRoomUserCharacterByTeamSlot(EPBTeamType TeamType, int32 SlotIdx, bool bPlayStartAnimOnLoaded, bool bVisibleOnLoaded)
{
	auto UserManager = UPBRoomUserInfoManager::Get(OwningPlayer);
	if (UserManager == nullptr)
		return;

	FPBRoomSlotInfo& RoomSlotInfo = UserManager->GetUserSlotInfoByTeam(TeamType, SlotIdx);
	if (RoomSlotInfo.IsValidBase())
	{
		SpawnRoomUserCharacter(RoomSlotInfo, bPlayStartAnimOnLoaded, bVisibleOnLoaded);
	}
}

void UPBCharacterScene::SpawnRoomUserCharacter(FPBRoomSlotInfo& RoomSlotInfo, bool bPlayStartAnimOnLoaded, bool bVisibleOnLoaded)
{	
	FCharacterSceneSpawnInfo* SpawnedSlotInfo = FindCachedRoomSpawnInfo(RoomSlotInfo.TeamType, RoomSlotInfo.TeamSlotIdx);
	
	if (SpawnedSlotInfo && SpawnedSlotInfo->DefaultPawn)
	{
		// 이미 스폰 되어 있다면 visibility 만 설정함
		if (SpawnedSlotInfo->UserBaseInfoPtr == &RoomSlotInfo.UserBaseInfo)
		{
			SpawnedSlotInfo->DefaultPawn->ForceMeshVisibilityWithWeapon(bVisibleOnLoaded, false);
			if (bPlayStartAnimOnLoaded)
			{
				SpawnedSlotInfo->DefaultPawn->GetCurrentWeapon()->PlayWeaponAnimation(EWeaponAnimType::Lobby_WalkingStart);
			}
			return;
		}
		else
		{
			SpawnedSlotInfo->DefaultPawn->Destroy();
			SpawnedSlotInfo->DefaultPawn = nullptr;
		}
	}

	// 새것으로 스폰
	int32 CharItemID = RoomSlotInfo.UserBaseInfo.GetCharItemID(RoomSlotInfo.TeamType);
	int32 WeapItemID = RoomSlotInfo.UserBaseInfo.GetWeapItemID(EWeaponSlot::Primary);

	auto* Pawn = Cast<APBCharacter>(SpawnTeamCharacter(CharItemID, RoomSlotInfo.bMine, RoomSlotInfo.TeamType, RoomSlotInfo.TeamSlotIdx));
	if (Pawn)
	{
		SetCharacter(Pawn, CharItemID);
		EquipWeapon(Pawn, WeapItemID);

		if (bPlayStartAnimOnLoaded)
		{
			Pawn->GetCurrentWeapon()->PlayWeaponAnimation(EWeaponAnimType::Lobby_WalkingStart);
		}
		Pawn->ForceMeshVisibilityWithWeapon(bVisibleOnLoaded, false);

		PrintOnScreen(FString::Printf(TEXT("Spawn (%s / %s)"), *RoomSlotInfo.UserBaseInfo.NickName, *Pawn->GetClass()->GetName()));

		FCharacterSceneSpawnInfo SpawnInfo;
		SpawnInfo.DefaultPawn = Pawn;
		SpawnInfo.UserBaseInfoPtr = &RoomSlotInfo.UserBaseInfo;
		CachedRoomUserSpawnInfoMap.FindOrAdd(RoomSlotInfo.TeamType).FindOrAdd(RoomSlotInfo.TeamSlotIdx) = SpawnInfo;
	}
}

void UPBCharacterScene::EquipWeapon(APawn* TargetPawn, int32 WeapItemID)
{
	auto* Pawn = Cast<APBCharacter>(TargetPawn);
	if (Pawn)
	{
		// allow only primary weapon to equip.
		FPBWeaponTableData* TableData = UPBGameplayStatics::GetItemTableManager(OwningPlayer)->GetWeaponTableData(WeapItemID);
		if (TableData && TableData->WeaponSlotEnum == (int32)EWeaponSlot::Primary)
		{
			UE_LOG(LogSet, Log, TEXT("Set weapon item id (%d) at CharacterScene"), WeapItemID);

			Pawn->EquipWeaponItem(WeapItemID, TableData->WeaponSlotEnum);
		}
	}
}

APawn* UPBCharacterScene::SpawnTeamCharacter(int32 CharItemID, bool bLocalPlayer, EPBTeamType TeamType, int32 TeamSlotIdx)
{
	int32 SpotIdx = DetermineStartSpotIdx(bLocalPlayer);

	DetermineDefaultPawnClass(CharItemID, bLocalPlayer);
	
	APawn* Pawn = SpawnDefaultPawnAtStartSpot(TeamType, SpotIdx);

	return Pawn;
}

int32 UPBCharacterScene::DetermineStartSpotIdx(bool bLocalPlayer)
{
	// FString TagName = FString::Printf(TEXT("Team_%s%d"), *TeamShorName, SpotIdx);

	const int32 LocalPlayerStartSpot = 1;
	const int32 OtherPlayerStartSpot = 2;

	if (bLocalPlayer)
	{
		LastSpawnSpotIdx = LocalPlayerStartSpot;
	}
	else
	{
		LastSpawnSpotIdx = (++LastSpawnSpotIdx % 9); // 2 ~ 8
		LastSpawnSpotIdx = FMath::Max(LastSpawnSpotIdx, OtherPlayerStartSpot);
	}

	return LastSpawnSpotIdx;
}

void UPBCharacterScene::DetermineDefaultPawnClass(int32 CharItemID, bool bLocalPlayer)
{
	auto* LobbyMode = Cast<APBMode_LobbyMenu>(UGameplayStatics::GetGameMode(OwningPlayer));

	if (LobbyMode)
	{
		//if (bLocalPlayer)
		//{
		//	// Local character 의 경우 APBMVCharacter 로 설정되어있음.
		TSubclassOf<APawn> UseDefaultPawnClass = nullptr;
		LobbyMode->SetDefaultPawnClass(UseDefaultPawnClass);
		//}
		//else
		//{
		//	auto Mgr = UPBGameplayStatics::GetItemTableManager(OwningPlayer);
		//	if (Mgr)
		//	{
		//		auto Data = Mgr->GetCharacterTableData(CharItemID);
		//		if (Data && Data->BPClass)
		//		{
		//			LobbyMode->SetDefaultPawnClass(Data->BPClass);
		//		}
		//	}
		//}
	}
}

APawn* UPBCharacterScene::SpawnDefaultPawnAtStartSpot(EPBTeamType SpotTeamType, int32 SpotIdx)
{
	auto* LobbyMode = Cast<APBMode_LobbyMenu>(UGameplayStatics::GetGameMode(OwningPlayer));

	if (LobbyMode)
	{
		AActor* StartSpot = FindTeamStartSpot(SpotTeamType, SpotIdx);

		return LobbyMode->SpawnDefaultPawnFor(nullptr, StartSpot);
	}
	return nullptr;
}

AActor* UPBCharacterScene::FindTeamStartSpot(EPBTeamType SpotTeamType, int32 SpotIdx)
{
	const FString TeamShorName = GetTeamShortName(SpotTeamType);
	FString TagName = FString::Printf(TEXT("Team_%s%d"), *TeamShorName, SpotIdx);
	
	UWorld* World = GetWorld();

	// If incoming start is specified, then just use it
	if (World && !TagName.IsEmpty())
	{
		const FName SpotTagName = FName(*TagName);
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && Start->PlayerStartTag == SpotTagName)
			{
				return Start;
			}
		}
	}

	return World ? World->GetWorldSettings() : nullptr; // meaning of Actor located at FVector(0,0,0)
}


void UPBCharacterScene::SetCharacter(APawn* TargetPawn, int32 CharItemID)
{
	// Note : Local players use 'APBMVCharacter' class. Other players is 'APBCharacter'

	// allow only Local player
	auto Pawn = Cast<APBMVCharacter>(TargetPawn);
	if (Pawn)
	{
		UE_LOG(LogSet, Log, TEXT("Set character item id (%d) at CharacterScene"), CharItemID);

		auto DefaultObject = GetCharacterCDO(CharItemID);
		Pawn->SetMVCharacter(DefaultObject);
	}
}

class APBCharacter* UPBCharacterScene::GetCharacterCDO(int32 Id)
{
	auto Mgr = UPBGameplayStatics::GetItemTableManager(OwningPlayer);
	if (Mgr)
	{
		auto Data = Mgr->GetCharacterTableData(Id);
		if (Data && Data->BPClass)
		{
			return Cast<APBCharacter>(Data->BPClass->GetDefaultObject());
		}
	}

	return nullptr;
}
