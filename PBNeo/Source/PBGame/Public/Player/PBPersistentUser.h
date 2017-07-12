// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online/PBUserInfo.h"
#include "PBPersistentUser.generated.h"

UCLASS()
class UPBPersistentUser : public USaveGame
{
	GENERATED_UCLASS_BODY()

public:
	/** Loads user persistence data if it exists, creates an empty record otherwise. */
	static UPBPersistentUser* LoadPersistentUser(FString SlotName, const int32 UserIndex);

	/** Saves data if anything has changed. */
	void SaveIfDirty();

	DECLARE_EVENT(UPBPersistentUser, FUserSettingUpdatedEvt)
	FUserSettingUpdatedEvt OnUserSettingUpdated;

	/** Records the result of a match. */
	void AddMatchResult(int32 MatchKills, int32 MatchDeaths, bool bIsMatchWinner);

	/** needed because we can recreate the subsystem that stores it */
	void TellInputAboutKeybindings();

	int32 GetUserIndex() const;

	FORCEINLINE int32 GetKills() const
	{
		return Kills;
	}

	FORCEINLINE int32 GetDeaths() const
	{
		return Deaths;
	}

	FORCEINLINE int32 GetWins() const
	{
		return Wins;
	}

	FORCEINLINE int32 GetLosses() const
	{
		return Losses;
	}

	/** Is the y axis inverted? */
	FORCEINLINE bool GetInvertedYAxis() const
	{
		return bInvertedYAxis;
	}

	/** Setter for inverted y axis */
	void SetInvertedYAxis(bool bInvert);

	/** Getter for the aim sensitivity */
	FORCEINLINE float GetAimSensitivity() const
	{
		return AimSensitivity;
	}

	void SetAimSensitivity(float InSensitivity);

	/** Getter for the gamma correction */
	FORCEINLINE float GetGamma() const
	{
		return Gamma;
	}

	void SetGamma(float InGamma);

	FORCEINLINE int32 GetBotsCount() const
	{
		return BotsCount;
	}

	void SetBotsCount(int32 InCount);

	FORCEINLINE bool IsRecordingDemos() const
	{
		return bIsRecordingDemos;
	}

	void SetIsRecordingDemos(const bool InbIsRecordingDemos);

	FORCEINLINE FString GetName() const
	{
		return SlotName;
	}

	FPBUserBaseInfo& GetBaseUserInfo();
	void SetBaseUserInfo(FPBUserBaseInfo& UserInfo);


public:
	// Set the default item info to prevent the case that user has no weapon.
	void SetDefaultItemInfo();
private:
	void SetDefaultCharItems();
	void SetDefaultWeapItems();


protected:
	void SetToDefaults();

	/** Checks if the Inverted Mouse user setting is different from current */
	bool IsInvertedYAxisDirty() const;

	/** Triggers a save of this data. */
	void SavePersistentUser();

	/** Lifetime count of kills */
	UPROPERTY()
	int32 Kills;

	/** Lifetime count of deaths */
	UPROPERTY()
	int32 Deaths;

	/** Lifetime count of match wins */
	UPROPERTY()
	int32 Wins;

	/** Lifetime count of match losses */
	UPROPERTY()
	int32 Losses;

	/** how many bots join hosted game */
	UPROPERTY()
	int32 BotsCount;

	/** is recording demos? */
	UPROPERTY()
	bool bIsRecordingDemos;

	/** Holds the gamma correction setting */
	UPROPERTY()
	float Gamma;

	/** Holds the mouse sensitivity */
	UPROPERTY()
	float AimSensitivity;

	/** Is the y axis inverted or not? */
	UPROPERTY()
	bool bInvertedYAxis;

	UPROPERTY()
	FPBUserBaseInfo PersistentUserBaseInfo;

private:
	/** Internal.  True if data is changed but hasn't been saved. */
	bool bIsDirty;

	/** The string identifier used to save/load this persistent user. */
	FString SlotName;
	int32 UserIndex;
};