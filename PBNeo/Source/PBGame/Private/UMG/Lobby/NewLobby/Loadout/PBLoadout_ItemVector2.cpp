// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_ItemVector2.h"
#include "Loadout/PBLoadout_Item.h"
#include "UMG/PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UHorizontalBox, UPBLoadout_ItemVector2, ItemHorizontalBox);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemVector2, Item0);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemVector2, Item1);

UPBLoadout_ItemVector2::UPBLoadout_ItemVector2()
{

}



FReply UPBLoadout_ItemVector2::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReplay = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	// Set the initial focus target
	return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_Item0()).NativeReply;
}

void UPBLoadout_ItemVector2::SetItemsId(int32 ItemId0, int32 ItemId1)
{
	GetBPV_Item0()->SetItemId(ItemId0);
	GetBPV_Item1()->SetItemId(ItemId1);

}

void UPBLoadout_ItemVector2::BindEvents()
{
	BindPBButtonEvents<UPBLoadout_ItemVector2>(this, GetBPV_Item0());
	BindPBButtonEvents<UPBLoadout_ItemVector2>(this, GetBPV_Item1());
}

void UPBLoadout_ItemVector2::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	OnPBButtonReleased.Broadcast(Widget);
}

void UPBLoadout_ItemVector2::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	bool bIsHandled = false;

	if (Widget == GetBPV_Item0())
	{
		if (UPBUMGUtils::IsRight(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer() ,GetBPV_Item1());
			bIsHandled = true;
		}
	}
	else if (Widget == GetBPV_Item1())
	{
		if (UPBUMGUtils::IsLeft(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_Item0());
			bIsHandled = true;
		}
	}

	if (false == bIsHandled)
	{
		OnPBButtonGotNaviInput.Broadcast(this, PressedKey);
	}
}

void UPBLoadout_ItemVector2::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{
	OnPBButtonFocused.Broadcast(Widget, bFocused);
}

void UPBLoadout_ItemVector2::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}
