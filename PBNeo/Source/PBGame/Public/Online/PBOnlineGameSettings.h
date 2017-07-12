// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

/**
 * General session settings for a PB game
 */
class FPBOnlineSessionSettings : public FOnlineSessionSettings
{
public:

	FPBOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FPBOnlineSessionSettings() {}
};

/**
 * General search setting for a PB game
 */
class FPBOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	FPBOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FPBOnlineSearchSettings() {}
};

/**
 * Search settings for an empty dedicated server to host a match
 */
class FPBOnlineSearchSettingsEmptyDedicated : public FPBOnlineSearchSettings
{
public:
	FPBOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FPBOnlineSearchSettingsEmptyDedicated() {}
};
