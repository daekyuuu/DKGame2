// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Player/PBPersistentUser.h"
#include "PBLocalPlayer.generated.h"

class UWidget;

UCLASS(config=Engine, transient)
class UPBLocalPlayer : public ULocalPlayer
{
	GENERATED_UCLASS_BODY()

public:
	virtual void SetControllerId(int32 NewControllerId) override;

	virtual FString GetNickname() const;

	class UPBPersistentUser* GetPersistentUser() const;
	
	/** Initializes the PersistentUser */
	void LoadPersistentUser();

	TSharedPtr<SWidget> GetFousedWidgetBeforeControllerInvalidated();

private:
	/** Persistent user data stored between sessions (i.e. the user's savegame) */
	UPROPERTY()
	class UPBPersistentUser* PersistentUser;

	// Record the latest widget that has user focus before player controller is invalidated
	TSharedPtr<SWidget> FocusedWidgetBeforeControllerInvalidated;
};


