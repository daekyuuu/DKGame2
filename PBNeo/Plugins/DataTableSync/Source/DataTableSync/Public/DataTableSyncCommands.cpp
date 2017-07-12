// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DataTableSyncPrivatePCH.h"
#include "DataTableSyncCommands.h"

#define LOCTEXT_NAMESPACE "FDataTableSyncModule"

void FDataTableSyncCommands::RegisterCommands()
{
	UI_COMMAND(SyncTableToAllCommand, "DataTable Sync To All", "Execute SyncTableToAll action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SyncTableToWeaponCommand, "DataTable Sync To Weapon", "Execute SyncTableToWeapon action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SyncTableToCharacterCommand, "DataTable Sync To Character", "Execute SyncTableToCharacter action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SyncTableToGameModeCommand, "DataTable Sync To GameMode", "Execute SyncTableToGameMode action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SyncTableToGameStringCommand, "DataTable Sync To GameString", "Execute SyncTableToGameString action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SyncTableToGameMapCommand, "DataTable Sync To GameMap", "Execute SyncTableToGameMap action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
