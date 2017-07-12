// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPersistentUser.h"
#include "Player/PBLocalPlayer.h"
#include "PBGameUserSettings.h"

UPBPersistentUser::UPBPersistentUser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

void UPBPersistentUser::SetToDefaults()
{
	bIsDirty = false;

	bInvertedYAxis = false;
	AimSensitivity = 1.0f;
	Gamma = 2.2f;
	BotsCount = 1;
	bIsRecordingDemos = false;
}

bool UPBPersistentUser::IsInvertedYAxisDirty() const
{
	bool bIsInvertedYAxisDirty = false;
	if (GEngine)
	{
		TArray<APlayerController*> PlayerList;
		GEngine->GetAllLocalPlayerControllers(PlayerList);

		for (auto It = PlayerList.CreateIterator(); It; ++It)
		{
			APlayerController* PC = *It;
			if (!PC || !PC->Player || !PC->PlayerInput)
			{
				continue;
			}

			// Update key bindings for the current user only
			UPBLocalPlayer* LocalPlayer = Cast<UPBLocalPlayer>(PC->Player);
			if(!LocalPlayer || LocalPlayer->GetPersistentUser() != this)
			{
				continue;
			}

			bIsInvertedYAxisDirty |= PC->PlayerInput->GetInvertAxis("Lookup") != GetInvertedYAxis();
			bIsInvertedYAxisDirty |= PC->PlayerInput->GetInvertAxis("LookupRate") != GetInvertedYAxis();
		}
	}

	return bIsInvertedYAxisDirty;
}

void UPBPersistentUser::SavePersistentUser()
{
	UPBGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
	bIsDirty = false;
}

UPBPersistentUser* UPBPersistentUser::LoadPersistentUser(FString SlotName, const int32 UserIndex)
{
	UPBPersistentUser* Result = nullptr;
	
	// first set of player signins can happen before the UWorld exists, which means no OSS, which means no user names, which means no slotnames.
	// Persistent users aren't valid in this state.
	if (SlotName.Len() > 0)
	{	
		Result = Cast<UPBPersistentUser>(UPBGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (Result == NULL)
		{
			// if failed to load, create a new one
			Result = Cast<UPBPersistentUser>( UPBGameplayStatics::CreateSaveGameObject(UPBPersistentUser::StaticClass()) );
		}
		check(Result != NULL);
	
		Result->SlotName = SlotName;
		Result->UserIndex = UserIndex;

		Result->SetDefaultItemInfo();
	}

	return Result;
}

void UPBPersistentUser::SaveIfDirty()
{
	if (bIsDirty || IsInvertedYAxisDirty())
	{
		OnUserSettingUpdated.Broadcast();
		SavePersistentUser();
	}
}

void UPBPersistentUser::AddMatchResult(int32 MatchKills, int32 MatchDeaths, bool bIsMatchWinner)
{
	Kills += MatchKills;
	Deaths += MatchDeaths;
	
	if (bIsMatchWinner)
	{
		Wins++;
	}
	else
	{
		Losses++;
	}

	bIsDirty = true;
}

void UPBPersistentUser::TellInputAboutKeybindings()
{
	TArray<APlayerController*> PlayerList;
	GEngine->GetAllLocalPlayerControllers(PlayerList);

	for (auto It = PlayerList.CreateIterator(); It; ++It)
	{
		APlayerController* PC = *It;
		if (!PC || !PC->Player || !PC->PlayerInput)
		{
			continue;
		}

		// Update key bindings for the current user only
		UPBLocalPlayer* LocalPlayer = Cast<UPBLocalPlayer>(PC->Player);
		if(!LocalPlayer || LocalPlayer->GetPersistentUser() != this)
		{
			continue;
		}

		//invert it, and if does not equal our bool, invert it again
		if (PC->PlayerInput->GetInvertAxis("LookupRate") != GetInvertedYAxis())
		{
			PC->PlayerInput->InvertAxis("LookupRate");
		}

		if (PC->PlayerInput->GetInvertAxis("Lookup") != GetInvertedYAxis())
		{
			PC->PlayerInput->InvertAxis("Lookup");
		}
	}
}

int32 UPBPersistentUser::GetUserIndex() const
{
	return UserIndex;
}

void UPBPersistentUser::SetInvertedYAxis(bool bInvert)
{
	bIsDirty |= bInvertedYAxis != bInvert;

	bInvertedYAxis = bInvert;
}

void UPBPersistentUser::SetAimSensitivity(float InSensitivity)
{
	bIsDirty |= AimSensitivity != InSensitivity;

	AimSensitivity = InSensitivity;
}

void UPBPersistentUser::SetGamma(float InGamma)
{
	bIsDirty |= Gamma != InGamma;

	Gamma = InGamma;
}

void UPBPersistentUser::SetBotsCount(int32 InCount)
{
	bIsDirty |= BotsCount != InCount;

	BotsCount = InCount;
}

void UPBPersistentUser::SetIsRecordingDemos(const bool InbIsRecordingDemos)
{
	bIsDirty |= bIsRecordingDemos != InbIsRecordingDemos;

	bIsRecordingDemos = InbIsRecordingDemos;
}

FPBUserBaseInfo& UPBPersistentUser::GetBaseUserInfo()
{
	return PersistentUserBaseInfo;
}

void UPBPersistentUser::SetBaseUserInfo(FPBUserBaseInfo& UserInfo)
{
	bIsDirty |= (PersistentUserBaseInfo != UserInfo);

	PersistentUserBaseInfo = UserInfo;
}

void UPBPersistentUser::SetDefaultItemInfo()
{
	UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());

	SetDefaultCharItems();
	SetDefaultWeapItems();

}

void UPBPersistentUser::SetDefaultCharItems()
{
	UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());
	if (nullptr == Settings)
	{
		return;
	}

	// Character Item Ids( alpha, bravo )
	auto& EquippedChars = PersistentUserBaseInfo.EquippedCharItems;
	const auto& DefaultsChars = Settings->DefaultCharItems;

	if (EquippedChars.Num() < DefaultsChars.Num())
	{
		EquippedChars.SetNumZeroed(DefaultsChars.Num());
	}

	// If there's no character, set the default character.
	for (int i = 0; i < DefaultsChars.Num(); ++i)
	{
		if (EquippedChars[i] == 0)
		{
			EquippedChars[i] = DefaultsChars[i];
		}
	}
}

void UPBPersistentUser::SetDefaultWeapItems()
{
	UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());
	if (nullptr == Settings)
	{
		return;
	}

	// Weapon Item Ids( index mean the weapon slot. )
	auto& EquippedWeaps = PersistentUserBaseInfo.EquippedWeapItems;
	const auto& DefaultsWeaps = Settings->DefaultWeapItems;

	if (EquippedWeaps.Num() < DefaultsWeaps.Num())
	{
		EquippedWeaps.SetNumZeroed(DefaultsWeaps.Num());
	}

	// If there's no weapon at the slot, then set the default weapon to empty slot.
	for (int i = 0; i < DefaultsWeaps.Num(); ++i)
	{
		if (EquippedWeaps[i] == 0)
		{
			EquippedWeaps[i] = DefaultsWeaps[i];
		}
	}

}
