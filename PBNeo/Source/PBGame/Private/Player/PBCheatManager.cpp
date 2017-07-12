// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBCheatManager.h"
#include "Player/PBPlayerState.h"
#include "Bots/PBAIController.h"
#include "PBHUDWidget.h"
#include "Table/Item/PBItemTableManager.h"
#include "Table/Game/PBGameTableManager.h"
#include "Weapons/PBWeapon.h"
#include "PBMode_LobbyMenu.h"
#include "PBCharacterScene.h"
#include "Online/PBRoomUserInfoManager.h"
#include "Animation/AnimMontage.h"

bool UPBCheatManager::DebugPierceToggle = false;
bool UPBCheatManager::EnabledDebugFallOffDamage = false;
bool UPBCheatManager::EnabledDebugFallOffStatus = false;

UPBCheatManager::UPBCheatManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPBCheatManager::CheatInfiniteAmmo()
{
	APBPlayerController* MyPC = GetPBPlayerController();
	if (!MyPC) return;

	if (MyPC->Role == ROLE_Authority)
	{
		MyPC->SetInfiniteAmmo(!MyPC->HasInfiniteAmmo());
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatInfiniteClip()
{
	APBPlayerController* MyPC = GetPBPlayerController();
	if (!MyPC) return;

	if (MyPC->Role == ROLE_Authority)
	{
		MyPC->SetInfiniteClip(!MyPC->HasInfiniteClip());
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatGodMode()
{
	auto PC = GetPBPlayerController();
	if (!PC) return;

	if (PC->Role == ROLE_Authority)
	{
		PC->SetGodMode(!PC->HasGodMode());
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatHealthRegen()
{
	auto PC = GetPBPlayerController();
	if (!PC) return;

	if (PC->Role == ROLE_Authority)
	{
		PC->SetHealthRegen(!PC->HasHealthRegen());
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatSuicide()
{
	auto PC = GetPBPlayerController();
	if (!PC) return;

	if (PC->Role == ROLE_Authority)
	{
		PC->Suicide();
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatUnbeatable()
{
	APBCharacter* Character = GetPBCharacter();
	if (!Character) return;

	if (Character->Role == ROLE_Authority)
	{
		if (Character->HasUnbeatable())
		{
			Character->UnbeatableOff();
		}
		else
		{
			Character->UnbeatableOn();
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatSetHealth(int32 Value)
{
	APBCharacter* Character = GetPBCharacter();
	if (!Character) return;

	if (Character->Role == ROLE_Authority)
	{
		Character->Health = Value;
	}
	else
	{
		ServerCheat(PB_CUR_FUNC + FString(" ") + FString::FromInt(Value));
	}
}

void UPBCheatManager::CheatPauseMatch()
{
	APBPlayerController* MyPC = GetPBPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;

	if (MyPC->Role == ROLE_Authority)
	{
		APBGameState* const GS = Cast<APBGameState>(MyPC->GetWorld()->GetGameState());
		if (GS && GS->Role == ROLE_Authority)
		{
			GS->bTimerPaused = !GS->bTimerPaused;
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatSetMatchTime(int32 Value)
{
	if (Value <= 0)
	{
		return;
	}

	APBPlayerController* MyPC = GetPBPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;

	if (MyPC->Role == ROLE_Authority)
	{
		APBGameState* const GS = Cast<APBGameState>(MyPC->GetWorld()->GetGameState());
		if (GS && GS->Role == ROLE_Authority)
		{
			GS->RemainingTime = Value;
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatForceMatchStart()
{
	APBPlayerController* const MyPC = GetPBPlayerController();;
	if (!MyPC || MyPC->GetWorld() == nullptr) return;

	if (MyPC->Role == ROLE_Authority)
	{
		APBGameMode* const MyGame = MyPC->GetWorld()->GetAuthGameMode<APBGameMode>();
		if (MyGame && MyGame->GetMatchState() == MatchState::WaitingToStart)
		{
			MyGame->StartMatch();
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatSetTeam(EPBTeamType NewTeamType)
{
	APBPlayerController* MyPC = GetPBPlayerController();
	if (!MyPC) return;
	
	if (MyPC->Role == ROLE_Authority)
	{
		APBPlayerState* MyPlayerState = Cast<APBPlayerState>(MyPC->PlayerState);
		if (MyPlayerState)
		{
			MyPlayerState->SetTeamType(NewTeamType);
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::CheatSpawnBot()
{
	APBPlayerController* const MyPC = GetPBPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;

	if (MyPC->Role == ROLE_Authority)
	{
		APawn* const MyPawn = MyPC->GetPawn();
		APBGameMode* const MyGame = MyPC->GetWorld()->GetAuthGameMode<APBGameMode>();
		UWorld* World = MyPC->GetWorld();
		if (MyPawn && MyGame && World)
		{
			static int32 CheatBotNum = 50;
			APBAIController* PBAIController = MyGame->CreateBot(CheatBotNum++);
			MyGame->RestartPlayer(PBAIController);
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

void UPBCheatManager::SyncTableToCharacterBP()
{
	auto TableManager = UPBItemTableManager::Get(this);
	if (TableManager)
	{
		TableManager->ApplyDataToBlueprint(EPBTableType::Character);
	}
}

void UPBCheatManager::SyncTableToWeaponBP()
{
	auto TableManager = UPBItemTableManager::Get(this);
	if (TableManager)
	{
		TableManager->ApplyDataToBlueprint(EPBTableType::Weapon);
	}
}

void UPBCheatManager::SyncTableToGameModeBP()
{
	auto TableManager = UPBGameTableManager::Get(this);
	if (TableManager)
	{
		TableManager->ApplyDataToBlueprint(EPBTableType::GameMode);
	}
}

void UPBCheatManager::SyncTableToAllBP()
{
	SyncTableToCharacterBP();
	SyncTableToWeaponBP();
	SyncTableToGameModeBP();
}

void UPBCheatManager::CheatWeapon(int32 ItemID)
{
	APlayerController* const MyPC = GetOuterAPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;
	
	auto TM = UPBItemTableManager::Get(this);
	if (TM == nullptr)
	{
		return;
	}

	if (MyPC->IsLocalController())
	{
		if (nullptr == TM->GetWeaponTableData(ItemID))
		{
			// if ItemID is invalid, display the weapon list
			UPBGameplayStatics::DisplayWeaponItemList(this);
			return;
		}
	}

	if (MyPC->Role == ROLE_Authority)
	{
		// in other game mode, just equip the owning character's weapon

		APBCharacter* const MyPawn = Cast<APBCharacter>(MyPC->GetPawn());

		// equip weapon
		TM->ApplyWeapItemToPawn(MyPawn, ItemID, true, 0);

	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}

}

void UPBCheatManager::CheatWeaponItemList()
{
	//APBPlayerController* const MyPC = GetPBPlayerController();
	//if (!MyPC) return;

	UPBGameplayStatics::DisplayWeaponItemList(this);
}

// Only Standalone game
void UPBCheatManager::RestartGame(FString MapPathName, bool bAbsolute)
{
	APBPlayerController* const MyPC = GetPBPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;

	if (MyPC->Role == ROLE_Authority)
	{
		APawn* const MyPawn = MyPC->GetPawn();
		APBGameMode* const MyGame = MyPC->GetWorld()->GetAuthGameMode<APBGameMode>();
		UWorld* World = MyPC->GetWorld();
		if (MyPawn && MyGame && World)
		{
			if (!MapPathName.Contains(TEXT("Maps/"), ESearchCase::CaseSensitive))
			{
				MapPathName = TEXT("Maps/") + MapPathName;
			}
			if (!MapPathName.Contains(TEXT("Game/")))
			{
				MapPathName = TEXT("Game/") + MapPathName;
			}
			if (!MapPathName.StartsWith(TEXT("/")))
			{
				MapPathName = TEXT("/") + MapPathName;
			}

			World->ServerTravel(*MapPathName, false);
		}
	}
	else
	{
		ServerCheat(PB_CUR_FUNC);
	}
}

FName PreLevelName = "";
FName NewLevelName = "";

void UPBCheatManager::LoadMap(FString MapPathName)
{	
	PreLevelName = *UGameplayStatics::GetCurrentLevelName(this);

	for (ULevelStreaming* LevelStreaming : GetWorld()->StreamingLevels)
	{
		if (LevelStreaming && LevelStreaming->IsLevelLoaded() && LevelStreaming->IsLevelVisible())
		{
			PreLevelName = LevelStreaming->PackageNameToLoad;
			PrintOnScreen(FString::Printf(TEXT("Name:%s"), *PreLevelName.ToString()));
		}
	}

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "LoadMapComplete";
	LatentInfo.UUID = 2;
	LatentInfo.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(this, *MapPathName, false, true, LatentInfo);
	NewLevelName = *MapPathName;
}

void UPBCheatManager::LoadMapComplete()
{
	FTimerHandle TimerHandle_OnPostLoadMap;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_OnPostLoadMap, this, &UPBCheatManager::UnloadMap, 3.f, false);
}

void UPBCheatManager::UnloadMap()
{
	ULevelStreaming* PendingLevel = UGameplayStatics::GetStreamingLevel(this, NewLevelName);
	if (PendingLevel)
	{
		PendingLevel->bShouldBeVisible = true;
	}
	GetWorld()->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);

	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevel(this, PreLevelName, LatentInfo);

	APBPlayerController* const MyPC = GetPBPlayerController();
	if (!MyPC || MyPC->GetWorld() == nullptr) return;
	MyPC->RestartPlayerCharacter();
}

void UPBCheatManager::PlayAnim(FString MontagePathName)
{
	APBPlayerController* const MyPC = GetPBPlayerController();
	if (!MyPC) return;	

	if (!MontagePathName.Contains(TEXT("Game/")))
	{
		MontagePathName = TEXT("Game/") + MontagePathName;
	}	
	if (!MontagePathName.StartsWith(TEXT("/")))
	{
		MontagePathName = TEXT("/") + MontagePathName;
	}

	UAnimMontage* AnimMontage = LoadObject<UAnimMontage>(nullptr, *MontagePathName);
	if (AnimMontage == nullptr) return;

	APBCharacter* const MyPawn = Cast<APBCharacter>(MyPC->GetPawn());
	if (MyPawn)
	{
		MyPawn->ServerPlayAnimation(AnimMontage);
	}
}

class APBPlayerController* UPBCheatManager::GetPBPlayerController() const
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetOuterAPlayerController());
	if (PC && PC->IsValidLowLevel())
	{
		return PC;
	}

	return nullptr;
}

class APBCharacter* UPBCheatManager::GetPBCharacter() const
{
	if (GetPBPlayerController())
	{
		APBCharacter* Character = Cast<APBCharacter>(GetPBPlayerController()->GetCharacter());
		if (Character && Character->IsValidLowLevel())
		{
			return Character;
		}
	}

	return nullptr;
}

void UPBCheatManager::ServerCheat(const FString& Msg)
{
	GetPBPlayerController()->ServerCheat(Msg.Left(128));
}

void UPBCheatManager::DebugPierce()
{
	DebugPierceToggle = DebugPierceToggle ? false : true;

	if (DebugPierceToggle)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Debug Pierce Visible On"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Debug Pierce Visible Off"));
	}
}

void UPBCheatManager::DebugFallOffDamage()
{
	EnabledDebugFallOffDamage = !EnabledDebugFallOffDamage;

	if (GEngine)
	{
		FString StatusMessage = EnabledDebugFallOffDamage 
			? TEXT("Enabled DebugFallOffDamage") 
			: TEXT("Disabled DebugFallOffDamage");

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, StatusMessage);
	}
}

void UPBCheatManager::DebugFallOffStatus()
{
	EnabledDebugFallOffStatus = !EnabledDebugFallOffStatus;

	if (GEngine)
	{
		FString StatusMessage = EnabledDebugFallOffStatus
			? TEXT("Enabled DebugFallOffStatus")
			: TEXT("Disabled DebugFallOffStatus");

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, StatusMessage);
	}
}

void UPBCheatManager::DebugAddRoomSlots(int32 MaxSlotNum)
{
	auto UserManager = UPBRoomUserInfoManager::Get(this);
	if (UserManager)
	{
		UserManager->DebugAddRoomTeamSlots(MaxSlotNum);
	}
}
