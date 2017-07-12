// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_SoldierBtn.h"
#include "PBItemTable_Char.h"
#include "Image.h"
#include "TextBlock.h"
#include "PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_SoldierBtn, CharName0);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_SoldierBtn, CharName1);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_SoldierBtn, CharName2);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_SoldierBtn, CharName3);

PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_SoldierBtn, CharImg0);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_SoldierBtn, CharImg1);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_SoldierBtn, CharImg2);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_SoldierBtn, CharImg3);


void UPBLoadout_SoldierBtn::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyItemInfo();

}

void UPBLoadout_SoldierBtn::NativeDestruct()
{

	Super::NativeDestruct();
}

void UPBLoadout_SoldierBtn::AssignItemInfo(const FPBItemBaseInfo& InItemInfo)
{
	ItemInfo = InItemInfo;

}

int32 UPBLoadout_SoldierBtn::GetItemId() const
{
	return ItemInfo.ItemId;
}

void UPBLoadout_SoldierBtn::ApplyItemInfo()
{
	if (GetItemId() == 0)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	auto Res = GetResourcesFromId(GetItemId());

	SetCheck(ItemInfo.bEquipped);

	BuildPropertyArray();
	for (auto CharName : CharNameArray)
	{
		CharName->SetText(FText::FromString(Res.ItemName));
	}

	for (auto CharImg : CharImgArray)
	{
		UPBUMGUtils::SetOnlyBrushImage(CharImg, Res.ItemIcon);
	}
}

void UPBLoadout_SoldierBtn::BuildPropertyArray()
{
	CharImgArray.Empty();
	CharImgArray.Add(GetBPV_CharImg0());
	CharImgArray.Add(GetBPV_CharImg1());
	CharImgArray.Add(GetBPV_CharImg2());
	CharImgArray.Add(GetBPV_CharImg3());

	CharNameArray.Empty();
	CharNameArray.Add(GetBPV_CharName0());
	CharNameArray.Add(GetBPV_CharName1());
	CharNameArray.Add(GetBPV_CharName2());
	CharNameArray.Add(GetBPV_CharName3());
}

FPBResource_Loadout_Soldier UPBLoadout_SoldierBtn::GetResourcesFromId(int32 Id)
{
	FPBResource_Loadout_Soldier Res;

	if (Id != 0)
	{
		FPBCharacterTableData* TableData = UPBGameplayStatics::GetItemTableManager(this)->GetCharacterTableData(Id);

		if (TableData)
		{
			/// Set the icon
			FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconResShop_Char/";
			IconPath += TableData->IconResShop + "." + TableData->IconResShop;
			auto Tex = LoadObject<UTexture2D>(nullptr, *IconPath);
			if (Tex)
			{
				Res.ItemIcon = Tex;
			}

			/// TEMP. It should be changed to the code that get string from string table.
			FString ItemName = TableData->Memo;
			Res.ItemName = ItemName;

			if (Res.ItemName.IsEmpty())
			{
				Res.ItemName = TEXT("Invalid");
			}

		}


		if (Res.ItemIcon && !Res.ItemName.IsEmpty())
		{
			Res.bSucceedToSet = true;
		}
		else
		{
			Res.bSucceedToSet = false;
		}

		
	}
	else
	{
		Res.bSucceedToSet = false;
	}



	return Res;
}
