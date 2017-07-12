// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DataTableSyncPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "DataTableSyncStyle.h"
#include "DataTableSyncCommands.h"

#include "LevelEditor.h"

#include "PBTable.h"
#include "PBWeapon.h"
#include "PBItemTable_Weap.h"

static const FName DataTableSyncTabName("DataTableSync");

#define LOCTEXT_NAMESPACE "FDataTableSyncModule"

void FDataTableSyncModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FDataTableSyncStyle::Initialize();
	FDataTableSyncStyle::ReloadTextures();

	FDataTableSyncCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToAllCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToAll),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToWeaponCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToWeapon),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToCharacterCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToCharacter),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToGameModeCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToGameMode),
		FCanExecuteAction());
		
	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToGameStringCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToGameString),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FDataTableSyncCommands::Get().SyncTableToGameMapCommand,
		FExecuteAction::CreateRaw(this, &FDataTableSyncModule::CallbackSyncTableToGameMap),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDataTableSyncModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FDataTableSyncModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FDataTableSyncStyle::Shutdown();

	FDataTableSyncCommands::Unregister();
}

void FDataTableSyncModule::CallbackSyncTableToAll()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;
		
		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To All"));
}

void FDataTableSyncModule::CallbackSyncTableToWeapon()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;

		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				if (Data->TableType != EPBTableType::Weapon)
					continue;

				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To Weapon"));
}

void FDataTableSyncModule::CallbackSyncTableToCharacter()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;

		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				if (Data->TableType != EPBTableType::Character)
					continue;

				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To Character"));
}

void FDataTableSyncModule::CallbackSyncTableToGameMode()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;

		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				if (Data->TableType != EPBTableType::GameMode)
					continue;

				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To GameMode"));
}

void FDataTableSyncModule::CallbackSyncTableToGameString()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;

		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				if (Data->TableType != EPBTableType::GameString)
					continue;

				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To GameString"));
}

void FDataTableSyncModule::CallbackSyncTableToGameMap()
{
	for (TObjectIterator<UDataTable> Itr; Itr; ++Itr)
	{
		auto CurTable = *Itr;

		for (auto& Elem : CurTable->RowMap)
		{
			FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
			if (Data)
			{
				if (Data->TableType != EPBTableType::GameMap)
					continue;

				Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
			}
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DataTable Sync Message", "DataTable Sync Table To GameMap"));
}

void FDataTableSyncModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateStatic(&FDataTableSyncModule::MakeComboButtonMenu, PluginCommands),
		LOCTEXT("DataTableSync", "DataTableSync"),
		LOCTEXT("DataTableSync_ToolTip", "DataTableSync Options"),
		FSlateIcon(FDataTableSyncStyle::GetStyleSetName(), "DataTableSync.DefaultIcon"),
		false
	);
}

TSharedRef<SWidget> FDataTableSyncModule::MakeComboButtonMenu(TSharedPtr<FUICommandList> CommandList)
{
	FMenuBuilder MenuBuilder(true, CommandList);

	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToAllCommand);
	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToWeaponCommand);
	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToCharacterCommand);
	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToGameModeCommand);
	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToGameStringCommand);
	MenuBuilder.AddMenuEntry(FDataTableSyncCommands::Get().SyncTableToGameMapCommand);

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDataTableSyncModule, DataTableSync)