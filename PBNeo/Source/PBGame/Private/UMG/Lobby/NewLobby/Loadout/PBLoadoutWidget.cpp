// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadoutWidget.h"
#include "PBLoadout_Main.h"
#include "PBLoadout_Sub.h"
#include "PBLoadout_Inventory.h"
#include "lobby/Page/PBLoadoutPage.h"
#include "WidgetSwitcher.h"


PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Main, UPBLoadoutWidget, Loadout_Main);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Sub, UPBLoadoutWidget, Loadout_Sub);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Inventory, UPBLoadoutWidget, Loadout_WeapInventory);

PBGetterWidgetBlueprintVariable_Implementation(UWidgetSwitcher, UPBLoadoutWidget, Loadout_Switcher);


void UPBLoadoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

FReply UPBLoadoutWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	if (GetBPV_Loadout_Main())
	{
		return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_Loadout_Main()).NativeReply;
	}

	return SuperReply;
}

void UPBLoadoutWidget::BindEvents()
{
	BindEventsOfLoadout_Main();
	BindEventsOfLoadout_Sub();	
	BindEventsOfLoadout_Inventory();

}

void UPBLoadoutWidget::BindEventsOfLoadout_Main()
{
	
	GetBPV_Loadout_Main()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonReleased);
	GetBPV_Loadout_Main()->OnPBButtonGotNaviInput.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonGotNaviInput);
	GetBPV_Loadout_Main()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonFocused);
	GetBPV_Loadout_Main()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonCanceled);

	GetBPV_Loadout_Main()->OnPBPresetButtonFocused.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBPresetButtonFocused);
	GetBPV_Loadout_Main()->OnPBPresetButtonReleased.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBPresetButtonReleased);

}

void UPBLoadoutWidget::BindEventsOfLoadout_Sub()
{

	GetBPV_Loadout_Sub()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonReleased);
	GetBPV_Loadout_Sub()->OnPBButtonGotNaviInput.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonGotNaviInput);
	GetBPV_Loadout_Sub()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonFocused);
	GetBPV_Loadout_Sub()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonCanceled);

	GetBPV_Loadout_Sub()->OnPBItemSlotReleased.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBItemSlotReleased);
}

void UPBLoadoutWidget::BindEventsOfLoadout_Inventory()
{
	GetBPV_Loadout_WeapInventory()->OnPBLobbyTabChanged.AddUniqueDynamic(this, &UPBLoadoutWidget::HandleInventoryTabChanged);
	GetBPV_Loadout_WeapInventory()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget::HandlePBButtonCanceled);

}

void UPBLoadoutWidget::HandlePBButtonReleased(class UPBUserWidget* Widget)
{

}

void UPBLoadoutWidget::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{

}

void UPBLoadoutWidget::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

}

void UPBLoadoutWidget::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	if (Widget == GetBPV_Loadout_Sub())
	{
		GetBPV_Loadout_Switcher()->SetActiveWidget(GetBPV_Loadout_Main());
		NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_Loadout_Main());

	}
	else if (Widget == GetBPV_Loadout_WeapInventory())
	{
		GetBPV_Loadout_Switcher()->SetActiveWidget(GetBPV_Loadout_Sub());
		NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_Loadout_Sub());
	}
	else
	{
		PopPage();

	}
}

void UPBLoadoutWidget::HandlePBPresetButtonFocused(int32 SlotIndex)
{
	// seungyoon.ji
	// TODO: Get the weapon list at preset slot from the loadout page.
	// And notify it to the loadout_main

	auto P = GetOwningPage<UPBLoadoutPage>();
	if (P)
	{
		auto ItemList = P->GetPresetItemsAtSlot(SlotIndex);
		GetBPV_Loadout_Main()->NotifyItemListReceived(ItemList);
	}

}

void UPBLoadoutWidget::HandlePBPresetButtonReleased(int32 SlotIndex)
{
	auto P = GetOwningPage<UPBLoadoutPage>();
	if (P)
	{
		// Notify item list to the sub widget
		auto ItemList = P->GetPresetItemsAtSlot(SlotIndex);
		GetBPV_Loadout_Sub()->NotifyItemListReceived(ItemList);

		// Set Switcher target to the Loadout_Sub
		GetBPV_Loadout_Switcher()->SetActiveWidget(GetBPV_Loadout_Sub());

		// Navigate the user focus
		NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_Loadout_Sub());


	}
}

void UPBLoadoutWidget::HandlePBItemSlotReleased(int32 ItemId)
{
	auto P = GetOwningPage<UPBLoadoutPage>();
	if (P)
	{
		EWeaponType OutWeaponType = EWeaponType::Max;
		EWeaponSlot OutWeaponSlot = EWeaponSlot::Max;
		P->GetWeaponTypeAndSlotFromId(ItemId, OutWeaponType, OutWeaponSlot);

		// This Inventory is only for the Main, Sub Weapons.
		if (OutWeaponSlot == EWeaponSlot::Primary || OutWeaponSlot == EWeaponSlot::Secondary)
		{
			// Set the switcher target to the Inventory widget
			GetBPV_Loadout_Switcher()->SetActiveWidget(GetBPV_Loadout_WeapInventory());

			// Set the inventory's tab index according to the weapon type.
			GetBPV_Loadout_WeapInventory()->SetLobbyTabIndex((int32)OutWeaponType);

		}

	}

}

void UPBLoadoutWidget::HandleInventoryTabChanged(int32 SlotIndex)
{
	auto P = GetOwningPage<UPBLoadoutPage>();
	if (P)
	{
		// seungyoon.ji
		// HACK: I assume that Inventory Tab's SlotIndex will follow the EWeaponType order!
		EWeaponType WeaponType = (EWeaponType)SlotIndex;
		auto ItemList  = P->GetWeaponListHasSameType(WeaponType);

		// Notify item list to the Inventory widget
		GetBPV_Loadout_WeapInventory()->NotifyItemListReceived(ItemList);

	}
	

}
