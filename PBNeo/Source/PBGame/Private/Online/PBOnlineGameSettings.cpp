// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBOnlineGameSettings.h"


FPBOnlineSessionSettings::FPBOnlineSessionSettings(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	NumPublicConnections = MaxNumPlayers;
	if (NumPublicConnections < 0)
	{
		NumPublicConnections = 0;
	}
	NumPrivateConnections = 0;
	bIsLANMatch = bIsLAN;
	bShouldAdvertise = true;
	bAllowJoinInProgress = true;
	bAllowInvites = true;
	bUsesPresence = bIsPresence;
	bAllowJoinViaPresence = true;
	bAllowJoinViaPresenceFriendsOnly = false;
}

FPBOnlineSearchSettings::FPBOnlineSearchSettings(bool bSearchingLAN, bool bSearchingPresence)
{
	bIsLanQuery = bSearchingLAN;
	MaxSearchResults = 10;
	PingBucketSize = 50;

	if (bSearchingPresence)
	{
		QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	}
}

FPBOnlineSearchSettingsEmptyDedicated::FPBOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN, bool bSearchingPresence) :
	FPBOnlineSearchSettings(bSearchingLAN, bSearchingPresence)
{
	QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
	QuerySettings.Set(SEARCH_EMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);
}
