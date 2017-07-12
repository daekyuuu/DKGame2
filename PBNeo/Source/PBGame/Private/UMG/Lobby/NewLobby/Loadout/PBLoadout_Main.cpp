// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_Main.h"

#include "PBUMGUtils.h"
#include "UMG/Lobby/NewLobby/Loadout/PBLoadout_PresetButton.h"


PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBLoadout_Main, PresetButtonCanvas);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_ItemList, UPBLoadout_Main, LoadoutItemList);


UPBLoadout_Main::UPBLoadout_Main()
{
}


FReply UPBLoadout_Main::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	if (GetFirstButton())
	{
		return TransferUserFocusThroughReply(InFocusEvent, E, GetFirstButton()).NativeReply;
	}

	return SuperReply;
}

void UPBLoadout_Main::NotifyItemListReceived(const TArray<int32>& ItemList)
{
	GetBPV_LoadoutItemList()->NotifyItemListReceived(ItemList);
}

void UPBLoadout_Main::BindEvents()
{
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_PresetButtonCanvas());
	for (auto Child : Children)
	{
		auto PresetButton = Cast<UPBLoadout_PresetButton>(Child);
		if (PresetButton)
		{
			BindPBButtonEvents<UPBLoadout_Main>(this, PresetButton);
		}
	}
}

void UPBLoadout_Main::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	// Relay the "preset button released event" to the parent

	auto PresetButton = Cast<UPBLoadout_PresetButton>(Widget);
	if (PresetButton)
	{
		int32 Index = PresetButton->PresetSlotIndex;
		OnPBPresetButtonReleased.Broadcast(Index);

	}
}

void UPBLoadout_Main::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	if (true == UPBUMGUtils::IsUp(PressedKey))
	{
		UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_PresetButtonCanvas(), true);
	}
	else if (true == UPBUMGUtils::IsDown(PressedKey))
	{
		UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_PresetButtonCanvas(), false);
	}

	else if (true == UPBUMGUtils::IsRight(PressedKey))
	{

	}
	else if (true == UPBUMGUtils::IsLeft(PressedKey))
	{

	}
}

void UPBLoadout_Main::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

	// Relay the "preset button focused event" to the parent

	auto PresetButton = Cast<UPBLoadout_PresetButton>(Widget);
	if (PresetButton)
	{
		int32 Index = PresetButton->PresetSlotIndex;
		OnPBPresetButtonFocused.Broadcast(Index);

	}

}

void UPBLoadout_Main::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{

	// Relay the Cancel button event to the owner widget
	OnPBButtonCanceled.Broadcast(this);

}

class UPBUserWidget* UPBLoadout_Main::GetFirstButton()
{
	if (GetBPV_PresetButtonCanvas()->GetChildrenCount() > 0)
	{
		return Cast<UPBUserWidget>(GetBPV_PresetButtonCanvas()->GetChildAt(0));
	}

	return nullptr;

}
