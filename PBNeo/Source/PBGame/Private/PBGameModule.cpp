// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBGameDelegates.h"
#include "AssetRegistryModule.h"
#include "IAssetRegistry.h"

class FPBGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		InitializePBGameDelegates();
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, PBGame, "PBGame");

DEFINE_LOG_CATEGORY(LogPB);
DEFINE_LOG_CATEGORY(LogPBWeapon);
DEFINE_LOG_CATEGORY(LogUI);
DEFINE_LOG_CATEGORY(LogCharacter);
DEFINE_LOG_CATEGORY(LogWeaponProj);
DEFINE_LOG_CATEGORY(LogRoomSlot);
DEFINE_LOG_CATEGORY(LogSet);