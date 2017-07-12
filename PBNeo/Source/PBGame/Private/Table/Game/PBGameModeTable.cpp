// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBGameModeTable.h"
#include "Mode/PBGameMode.h"


void FPBGameModeTableData::LoadDefaultObject()
{
	if (BPClass)
	{
		BPClass->GetDefaultObject();
	}
}

void FPBGameModeTableData::ApplyDataToDefaultObject(int32 ID)
{
	// Sync TableData To Blueprint (테이블값을 블루프린트에 덮어 씌운다)

	if (BPClass)
	{
		auto DefaultClass = Cast<APBGameMode>(BPClass->GetDefaultObject());
		if (DefaultClass)
		{
			DefaultClass->SetItemID(ID);
			bool bResult = DefaultClass->ApplyTableData(this);

			BPClass->MarkPackageDirty();
#if WITH_EDITOR
			BPClass->PostEditChange();
#endif
		}
	}
}

FPBGameModeTableData* UPBGameModeTable::GetData(int32 ItemID)
{
	FPBGameModeTableData* Data = nullptr;
	Data = Table->FindRow<FPBGameModeTableData>(*FString::FromInt(ItemID), PBTABLE_KEY_STRING);
	return Data;
}

void UPBGameModeTable::ApplyDataTo(int32 ItemID, class APBGameMode* GameMode)
{
	FPBGameModeTableData* Data = GetData(ItemID);
	if (Data)
	{
		GameMode->SetItemID(ItemID);
		GameMode->ApplyTableData(Data);
	}
}
