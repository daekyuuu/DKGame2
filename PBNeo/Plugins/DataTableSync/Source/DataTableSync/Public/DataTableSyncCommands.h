// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "DataTableSyncStyle.h"

class FDataTableSyncCommands : public TCommands<FDataTableSyncCommands>
{
public:

	FDataTableSyncCommands()
		: TCommands<FDataTableSyncCommands>(TEXT("DataTableSync"), NSLOCTEXT("Contexts", "DataTableSync", "DataTableSync Plugin"), NAME_None, FDataTableSyncStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > SyncTableToAllCommand;
	TSharedPtr< FUICommandInfo > SyncTableToWeaponCommand;
	TSharedPtr< FUICommandInfo > SyncTableToCharacterCommand;
	TSharedPtr< FUICommandInfo > SyncTableToGameModeCommand;
	TSharedPtr< FUICommandInfo > SyncTableToGameStringCommand;
	TSharedPtr< FUICommandInfo > SyncTableToGameMapCommand;
};
