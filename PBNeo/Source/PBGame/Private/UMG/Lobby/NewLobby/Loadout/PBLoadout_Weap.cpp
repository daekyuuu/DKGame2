// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_Weap.h"
#include "PBSwitcherBasedButton.h"
#include "PBUMGUtils.h"
#include "TextBlock.h"
#include "Image.h"
#include "Table/Item/PBItemTable_Weap.h"
#include "Table/Item/PBItemTableManager.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Weap, WeapName0);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Weap, WeapName1);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Weap, WeapName2);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Weap, WeapName3);

PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Weap, WeapImg0);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Weap, WeapImg1);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Weap, WeapImg2);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Weap, WeapImg3);


UPBLoadout_Weap::UPBLoadout_Weap()
{
	ItemId = 0;
	MyDepth = 2;

}

void UPBLoadout_Weap::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_WeapName0();
	GetBPV_WeapName1();
	GetBPV_WeapName2();
	GetBPV_WeapName3();

	GetBPV_WeapImg0();
	GetBPV_WeapImg1();
	GetBPV_WeapImg2();
	GetBPV_WeapImg3();



}

void UPBLoadout_Weap::NativeDestruct()
{

	Super::NativeDestruct();
}

void UPBLoadout_Weap::UpdateButtonLook()
{

	Super::UpdateButtonLook();
}

FReply UPBLoadout_Weap::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UPBLoadout_Weap::NotifyDepthChanged(int32 NewDepth)
{
	Super::NotifyDepthChanged(NewDepth);
}

bool UPBLoadout_Weap::IsNavigatable() const
{
	return bIsFocusable && ItemId && (GetVisibility() != ESlateVisibility::Hidden);
}

void UPBLoadout_Weap::AssignItemInfo(const FPBItemBaseInfo& Info)
{
	ItemId = Info.ItemId;
	
	OnItemInfoAssigned(Info);
}

int32 UPBLoadout_Weap::GetItemId() const
{
	return ItemId;
}

void UPBLoadout_Weap::OnItemInfoAssigned(const FPBItemBaseInfo& Info)
{
	if (Info.ItemId == 0)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	auto Res = GetResourcesFromId(ItemId);

	SetCheck(Info.bEquipped);

	BuildPropertyArray();
	for (auto WeapName : WeapNameArray)
	{
		WeapName->SetText(FText::FromString(Res.ItemName));
	}

	for (auto WeapImg : WeapImgArray)
	{
		UPBUMGUtils::SetOnlyBrushImage(WeapImg, Res.ItemIcon);
	}


}

FPBResource_Loadout_Weap UPBLoadout_Weap::GetResourcesFromId(int32 Id)
{
	FPBResource_Loadout_Weap Res;

	if (Id != 0)
	{
		FPBWeaponTableData* TableData = UPBGameplayStatics::GetItemTableManager(this)->GetWeaponTableData(Id);
		if (TableData)
		{
			/// Set the icon
			FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconResShop/";
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

void UPBLoadout_Weap::BuildPropertyArray()
{
	WeapImgArray.Empty();
	WeapImgArray.Add(GetBPV_WeapImg0());
	WeapImgArray.Add(GetBPV_WeapImg1());
	WeapImgArray.Add(GetBPV_WeapImg2());
	WeapImgArray.Add(GetBPV_WeapImg3());

	WeapNameArray.Empty();
	WeapNameArray.Add(GetBPV_WeapName0());
	WeapNameArray.Add(GetBPV_WeapName1());
	WeapNameArray.Add(GetBPV_WeapName2());
	WeapNameArray.Add(GetBPV_WeapName3());

}

