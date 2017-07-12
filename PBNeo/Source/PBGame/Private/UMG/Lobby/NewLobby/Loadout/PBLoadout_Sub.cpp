// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_Sub.h"
#include "PBLoadout_ItemList.h"
#include "PBLoadout_Item.h"

PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_ItemList, UPBLoadout_Sub, LoadoutItemList);

UPBLoadout_Sub::UPBLoadout_Sub()
{
}


FReply UPBLoadout_Sub::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_LoadoutItemList()).NativeReply;

}

void UPBLoadout_Sub::NotifyItemListReceived(const TArray<int32>& ItemList)
{
	GetBPV_LoadoutItemList()->NotifyItemListReceived(ItemList);
}

void UPBLoadout_Sub::BindEvents()
{
	BindPBButtonEvents<UPBLoadout_Sub>(this, GetBPV_LoadoutItemList());
}

void UPBLoadout_Sub::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	UPBLoadout_Item* ItemButton = Cast<UPBLoadout_Item>(Widget);
	if (ItemButton)
	{
		OnPBItemSlotReleased.Broadcast(ItemButton->GetItemId());
	}
}

void UPBLoadout_Sub::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{

}

void UPBLoadout_Sub::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

}

void UPBLoadout_Sub::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}

