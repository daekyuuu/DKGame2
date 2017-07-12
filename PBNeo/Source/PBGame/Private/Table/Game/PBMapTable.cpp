// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMapTable.h"


bool UPBMapTable::Load(FString TablePath)
{
	if (!Super::Load(TablePath))
		return false;

	// 게임 모드별로 맵 리스트 저장함

	// This follows the approach of UDataTable::GetAllRows to get all rows but also along with the keys.
	TMap<int32, FPBMapTableData*> TableIdxToRows;
	for (auto& Elem : Table->RowMap)
	{
		ensure(Table->RowStruct && Table->RowStruct->IsChildOf(FPBMapTableData::StaticStruct()));
		TableIdxToRows.Add(FCString::Atoi(*(Elem.Key.ToString())), reinterpret_cast<FPBMapTableData*>(Elem.Value));
	}

	for (auto& Elem : TableIdxToRows)
	{
		if (Elem.Value)
		{
			FMapTableDataList* MapsPtr = ModeMaps.Find(Elem.Value->ModeShortName);
			if (MapsPtr)
			{
				MapsPtr->Add(Elem.Key, Elem.Value);
			}
			else
			{
				FMapTableDataList Maps;
				Maps.Add(Elem.Key, Elem.Value);
				ModeMaps.Add(Elem.Value->ModeShortName, Maps);
			}
		}
	}
	return true;
}

FPBMapTableData* UPBMapTable::GetData(int32 ItemID)
{
	FPBMapTableData* Data = nullptr;
	Data = Table->FindRow<FPBMapTableData>(*FString::FromInt(ItemID), PBTABLE_KEY_STRING);
	return Data;
}

FMapTableDataList* UPBMapTable::GetDataByMode(FString GameMode)
{
	return ModeMaps.Find(GameMode);
}