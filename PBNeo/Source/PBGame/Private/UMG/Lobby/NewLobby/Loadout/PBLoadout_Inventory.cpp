// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_Inventory.h"
#include "PBLobbyTabWidget.h"
#include "PBLoadout_InventoryScrollBox.h"
#include "UMG/PBUMGUtils.h"


PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_InventoryScrollBox, UPBLoadout_Inventory, InventoryScrollBox);
PBGetterWidgetBlueprintVariable_Implementation(UPBLobbyTabWidget, UPBLoadout_Inventory, LobbyTab);


UPBLoadout_Inventory::UPBLoadout_Inventory()
{
}


FReply UPBLoadout_Inventory::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReplay = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	// Set the initial focus target
	return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_InventoryScrollBox()).NativeReply;
}

void UPBLoadout_Inventory::NotifyItemListReceived(const TArray<int32>& ItemList)
{
	GetBPV_InventoryScrollBox()->NotifyItemListReceived(ItemList);
}

void UPBLoadout_Inventory::SetLobbyTabIndex(int32 Index)
{
	GetBPV_LobbyTab()->JumpTo(Index);
}

void UPBLoadout_Inventory::BindEvents()
{
	GetBPV_LobbyTab()->OnPBLobbyTabChanged.AddUniqueDynamic(this, &UPBLoadout_Inventory::HandlePBLobbyTabChanged);
	GetBPV_LobbyTab()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_Inventory::HandlePBButtonCanceled);

	BindPBButtonEvents<UPBLoadout_Inventory>(this, GetBPV_InventoryScrollBox());

}


void UPBLoadout_Inventory::HandlePBButtonReleased(class UPBUserWidget* Widget)
{

}

void UPBLoadout_Inventory::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	if (UPBUMGUtils::IsL1(PressedKey))
	{
		GetBPV_LobbyTab()->RotateToLeft();
	}
	else if (UPBUMGUtils::IsR1(PressedKey))
	{
		GetBPV_LobbyTab()->RotateToRight();
	}
}

void UPBLoadout_Inventory::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

}

void UPBLoadout_Inventory::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}

void UPBLoadout_Inventory::HandlePBLobbyTabChanged(int32 SlotIndex)
{
	OnPBLobbyTabChanged.Broadcast(SlotIndex);
}
