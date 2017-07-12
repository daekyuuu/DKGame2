// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_ItemList.h"
#include "CanvasPanel.h"
#include "UMG/PBUMGUtils.h"
#include "UMG/Lobby/NewLobby/Loadout/PBLoadout_Item.h"


PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBLoadout_ItemList, ItemCanvas);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemList, LoadoutItemMain);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemList, LoadoutItemSub);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemList, LoadoutItemGrenade);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Item, UPBLoadout_ItemList, LoadoutItemSpecial);


UPBLoadout_ItemList::UPBLoadout_ItemList()
{
}


FReply UPBLoadout_ItemList::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	// Set the initial focus target!
	return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_LoadoutItemMain()).NativeReply;

}

void UPBLoadout_ItemList::NotifyItemListReceived(const TArray<int32>& ItemList)
{

	// Set the item id to each item widget
	GetBPV_LoadoutItemMain()->SetItemId(ItemList[(int32)EWeaponSlot::Primary]);
	GetBPV_LoadoutItemSub()->SetItemId(ItemList[(int32)EWeaponSlot::Secondary]);
	GetBPV_LoadoutItemGrenade()->SetItemId(ItemList[(int32)EWeaponSlot::Grenade]);
	GetBPV_LoadoutItemSpecial()->SetItemId(ItemList[(int32)EWeaponSlot::Special]);


}

void UPBLoadout_ItemList::BindEvents()
{
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_ItemCanvas());
	for (auto Child : Children)
	{
		auto Item = Cast<UPBLoadout_Item>(Child);
		if (Item)
		{
			BindPBButtonEvents<UPBLoadout_ItemList>(this, Item);

		}
	}
	
}

void UPBLoadout_ItemList::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	if (Widget)
	{
		OnPBButtonReleased.Broadcast(Widget);
	}
}

void UPBLoadout_ItemList::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{

	// seungyoon.ji
	// HACK: List Have only 4 widgets. Hard coded navigation control would be okay. 

	/**
	*	[MainItem]	[GrenadeItem]
	*
	*	[SubItem]	[SpecialItem]
	*/

	if (GetBPV_LoadoutItemMain() == Widget)
	{
		if (UPBUMGUtils::IsRight(PressedKey) )
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemGrenade());
		}
		else if (UPBUMGUtils::IsDown(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemSub());
		}
	}
	else if (GetBPV_LoadoutItemSub() == Widget)
	{
		if (UPBUMGUtils::IsRight(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemSpecial());

		}
		else if (UPBUMGUtils::IsUp(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemMain());

		}
	}
	else if (GetBPV_LoadoutItemGrenade() == Widget)
	{
		if (UPBUMGUtils::IsLeft(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemMain());

		}
		else if (UPBUMGUtils::IsDown(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemSpecial());

		}
	}
	else if (GetBPV_LoadoutItemSpecial() == Widget)
	{
		if (UPBUMGUtils::IsLeft(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemSub());

		}
		else if (UPBUMGUtils::IsUp(PressedKey))
		{
			NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_LoadoutItemGrenade());
		}
	}
}

void UPBLoadout_ItemList::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

}

void UPBLoadout_ItemList::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}
