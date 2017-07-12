// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBItemTable_Char.h"
#include "Player/PBCharacter.h"


void FPBCharacterTableData::LoadDefaultObject()
{
	if (BPClass)
	{
		BPClass->GetDefaultObject();
	}
}

void FPBCharacterTableData::ApplyDataToDefaultObject(int32 ID)
{
	// Sync TableData To Blueprint (테이블값을 블루프린트에 덮어 씌운다)

	if (BPClass)
	{
		auto DefaultClass = Cast<APBCharacter>(BPClass->GetDefaultObject());
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

FPBCharacterTableData* UPBItemTable_Char::GetData(int32 ItemID)
{
	if (IndexIsValid(ItemID))
	{
		FPBCharacterTableData* Data = nullptr;
		Data = Table->FindRow<FPBCharacterTableData>(*FString::FromInt(ItemID), PBTABLE_KEY_STRING);
		return Data;
	}

	return nullptr;
}

void UPBItemTable_Char::ApplyDataTo(APBCharacter* Pawn, int32 ItemID)
{
	FPBCharacterTableData* Data = GetData(ItemID);
	if (Data && IndexIsValid(ItemID))
	{
		Pawn->SetItemID(ItemID);
		Pawn->ApplyTableData(Data);
	}
}

