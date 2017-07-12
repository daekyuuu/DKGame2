// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "Weapons/PBWeapon.h"
#include "Weapons/Bullet/PBBulletBase.h"
#include "PBItemTable_Weap.h"

void FPBWeaponTableData::LoadDefaultObject()
{
	if (BPClass)
	{
		BPClass->GetDefaultObject();
	}
}

void FPBWeaponTableData::ApplyDataToDefaultObject(int32 ID)
{
	// Sync TableData To Blueprint (테이블값을 블루프린트에 덮어 씌운다)

	if (BPClass)
	{
		auto DefaultClass = Cast<APBWeapon>(BPClass->GetDefaultObject());
		if (DefaultClass)
		{
			DefaultClass->SetItemID(ID);
			bool bIsDirty = DefaultClass->ApplyTableData(this);

			if (bIsDirty)
			{
				BPClass->MarkPackageDirty();
#if WITH_EDITOR
				BPClass->PostEditChange();
#endif
			}

		}
	}
}

FPBWeaponTableData* UPBItemTable_Weap::GetData(int32 ID)
{
	if (IndexIsValid(ID))
	{
		FPBWeaponTableData* Data = nullptr;
		Data = Table->FindRow<FPBWeaponTableData>(*FString::FromInt(ID), PBTABLE_KEY_STRING);
		return Data;
	}

	return nullptr;
}

void UPBItemTable_Weap::ApplyDataTo(APBWeapon* Weapon, int32 ItemID)
{
	FPBWeaponTableData* Data = GetData(ItemID);
	if (Data && IndexIsValid(ItemID))
	{
		Weapon->SetItemID(ItemID);
		Weapon->ApplyTableData(Data);
	}
}
