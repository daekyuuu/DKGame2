// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"

#include "PBGameplayStatics.h"
#include "UMG/Lobby/NewLobby/Loadout/PBLoadout_Item.h"
#include "UMG/lobby/NewLobby/Loadout/PBLoadout_ItemSkin.h"
#include "Image.h"
#include "Textblock.h"
#include "UMG/PBUMGUtils.h"
#include "Table/Item/PBItemTableManager.h"
#include "Table/Item/PBItemTable_Weap.h"

PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Item, ItemImage);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBLoadout_Item, GradeImage);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Item, ItemNameText);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_Item, RemainingTimeText);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_ItemSkin, UPBLoadout_Item, Loadout_ItemSkin);


UPBLoadout_Item::UPBLoadout_Item()
{
}

void UPBLoadout_Item::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBLoadout_Item::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UPBLoadout_Item::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_Loadout_ItemSkin()).NativeReply;

}

void UPBLoadout_Item::SetItemId(int32 InItemId)
{
	// Set the item id
	ItemId = InItemId;

	// update properties related with visual things..
	auto TM = UPBGameplayStatics::GetItemTableManager(this);
	if (TM)
	{
		auto WeapData = TM->GetWeaponTableData(InItemId);
		if (WeapData)
		{

			// Set Image
			FString IconPath = "/Game/UMG/Img/Icon/IconResShop/";
			IconPath += WeapData->IconResShop + "." + WeapData->IconResShop;
			auto Tex = LoadObject<UTexture2D>(nullptr, *IconPath);
			if (Tex->IsValidLowLevel())
			{
				UPBUMGUtils::SetOnlyBrushImage(GetBPV_ItemImage(), Tex);
			}

			// Set Name
			GetBPV_ItemNameText()->SetText(FText::FromString(WeapData->Name));
		}
	}
}


void UPBLoadout_Item::BindEvents()
{
	BindPBButtonEvents<UPBLoadout_Item>(this, GetBPV_Loadout_ItemSkin());
}

void UPBLoadout_Item::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	OnPBButtonReleased.Broadcast(this);
}

void UPBLoadout_Item::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	OnPBButtonGotNaviInput.Broadcast(this, PressedKey);
}

void UPBLoadout_Item::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{
	// seungyoon.ji
	// TODO: Show the Item info??
}

void UPBLoadout_Item::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}

