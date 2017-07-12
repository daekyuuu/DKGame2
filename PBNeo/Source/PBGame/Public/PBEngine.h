// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBEngine.generated.h"

UCLASS()
class PBGAME_API UPBEngine : public UGameEngine
{
	GENERATED_UCLASS_BODY()

	/* Hook up specific callbacks */
	virtual void Init(IEngineLoop* InEngineLoop);

public:

	/**
	 * 	All regular engine handling, plus update PBKing state appropriately.
	 */
	virtual void HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) override;

	virtual bool LoadMap(FWorldContext& WorldContext, FURL URL, class UPendingNetGame* Pending, FString& Error) override;
	virtual bool SimpleLoadMap(FWorldContext& WorldContext, FURL URL, class UPendingNetGame* Pending, FString& Error);

	bool bOnceSimpleLoadMap;
};

