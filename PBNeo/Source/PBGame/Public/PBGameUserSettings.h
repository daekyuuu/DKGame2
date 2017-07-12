// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBGameUserSettings.generated.h"

UCLASS()
class UPBGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

	/** Applies all current user settings to the game and saves to permanent storage (e.g. file), optionally checking for command line overrides. */
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	int32 GetGraphicsQuality() const
	{
		return GraphicsQuality;
	}

	void SetGraphicsQuality(int32 InGraphicsQuality)
	{
		GraphicsQuality = InGraphicsQuality;
	}

	bool GetUsePierceWeapon() const
	{
		return UsePierceWeapon;
	}
	
	// interface UGameUserSettings
	virtual void SetToDefaults() override;

	UPROPERTY(config)
	TArray<int32> DefaultCharItems;		// Character Item ID list of Teams(Alpha, bravo)

	UPROPERTY(config)
	TArray<int32> DefaultWeapItems;		// Weapon slot Item ID list

	UPROPERTY(config)
	FString AccountID;					// Login Account ID

	UPROPERTY(config)
	FString AccountPW;					// Login Account Password

private:
	/**
	 * Graphics Quality
	 *	0 = Low
	 *	1 = High
	 */
	UPROPERTY(config)
	int32 GraphicsQuality;

	UPROPERTY(config)
	bool UsePierceWeapon;
};
