// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "FPBKillMarkProperties.h"

FHotKillerData::FHotKillerData()
{
	TargetPlayerState = nullptr;
	NumKills = 0;
	NumHotKillerGet = 0;
	NumSerialPayback = 0;
}


void FHotKillerData::ClearAllCount()
{
	NumKills = 0;
	NumHotKillerGet = 0;
	NumSerialPayback = 0;
}

FPBKillMarkProperties::FPBKillMarkProperties()
{
	Init();
}

void FPBKillMarkProperties::Init()
{
	ResetAll();
}

void FPBKillMarkProperties::ResetAll()
{
	NumSerialKills = 0;
	NumMeleeKills = 0;
	NumHeadKills = 0;
	bAchieveOneShotOnekill = false;
	PlayerWhoKilledMe = nullptr;
	FiringCnt = 0;


	HotKillerDatas.Empty();

}

void FPBKillMarkProperties::ResetRespawnClearProperties()
{
	NumSerialKills = 0;
	NumMeleeKills = 0;
	NumHeadKills = 0;
	bAchieveOneShotOnekill = false;
	FiringCnt = 0;

	for (int i = 0; i < HotKillerDatas.Num(); ++i)
	{
		HotKillerDatas[i].NumSerialPayback = 0;
	}
}



bool FPBKillMarkProperties::FindIndexOfHotKillerData(class APBPlayerState* PlayerState, int32& outIndex)
{
	if (nullptr == PlayerState) return false;

	for (int i = 0; i < HotKillerDatas.Num(); ++i)
	{
		if (HotKillerDatas[i].TargetPlayerState == PlayerState)
		{
			outIndex = i;
			return true;
		}
	}

	return false;
}

int32 FPBKillMarkProperties::IncreaseUserKill(class APBPlayerState* VictimPlayerState)
{

	if (VictimPlayerState)
	{
		int32 Index = 0;

		if (true == FindIndexOfHotKillerData(VictimPlayerState, Index))
		{
			HotKillerDatas[Index].NumKills++;
		}
		else
		{
			// make new one and add.
			FHotKillerData data;
			data.TargetPlayerState = VictimPlayerState;
			Index = HotKillerDatas.Add(data);

			HotKillerDatas[Index].NumKills++;
		}

		return HotKillerDatas[Index].NumKills;


	}

	return 0;



}

int32 FPBKillMarkProperties::IncreaseHotKiller(class APBPlayerState* VictimPlayerState)
{
	if (VictimPlayerState)
	{
		int32 Index = 0;
		if (true == FindIndexOfHotKillerData(VictimPlayerState, Index))
		{
			HotKillerDatas[Index].NumHotKillerGet++;
		}
		else
		{
			// make new one and add.
			FHotKillerData data;
			data.TargetPlayerState = VictimPlayerState;
			Index = HotKillerDatas.Add(data);

			HotKillerDatas[Index].NumHotKillerGet++;
		}

		return HotKillerDatas[Index].NumHotKillerGet;


	}

	return 0;
}

void FPBKillMarkProperties::ClearHotKillerData(class APBPlayerState* VictimPlayerState)
{
	if (VictimPlayerState)
	{
		int32 Index = 0;
		if (true == FindIndexOfHotKillerData(VictimPlayerState, Index))
		{
			HotKillerDatas[Index].ClearAllCount();
		}
		else
		{
			// make new one and add.
			FHotKillerData data;
			data.TargetPlayerState = VictimPlayerState;
			Index = HotKillerDatas.Add(data);

			HotKillerDatas[Index].ClearAllCount();
		}

	}

}

int32 FPBKillMarkProperties::IncreaseSerialPayback(class APBPlayerState* VictimPlayerState)
{
	if (VictimPlayerState)
	{
		int32 Index = 0;
		if (true == FindIndexOfHotKillerData(VictimPlayerState, Index))
		{
			HotKillerDatas[Index].NumSerialPayback++;
		}
		else
		{
			// make new one and add.
			FHotKillerData data;
			data.TargetPlayerState = VictimPlayerState;
			Index = HotKillerDatas.Add(data);

			HotKillerDatas[Index].NumSerialPayback++;
		}

		return HotKillerDatas[Index].NumSerialPayback;


	}

	return 0;
}

const FHotKillerData* FPBKillMarkProperties::GetHotkillerData(class APBPlayerState* VictimPlayerState)
{
	if (VictimPlayerState)
	{
		int32 Index = 0;
		if (true == FindIndexOfHotKillerData(VictimPlayerState, Index))
		{
			return &HotKillerDatas[Index];
		}
		else
		{
			// make new one and add.
			FHotKillerData data;
			data.TargetPlayerState = VictimPlayerState;
			Index = HotKillerDatas.Add(data);

			return &HotKillerDatas[Index];
		}

	}

	return nullptr;
}

