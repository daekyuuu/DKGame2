// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWidget;

class FDataTableSyncModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void CallbackSyncTableToAll();
	void CallbackSyncTableToWeapon();
	void CallbackSyncTableToCharacter();
	void CallbackSyncTableToGameMode();
	void CallbackSyncTableToGameString();
	void CallbackSyncTableToGameMap();
	
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	static TSharedRef<SWidget> MakeComboButtonMenu(TSharedPtr<FUICommandList> CommandList);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};