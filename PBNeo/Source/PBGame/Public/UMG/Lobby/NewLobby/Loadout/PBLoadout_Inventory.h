// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_Inventory : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UPBLoadout_InventoryScrollBox, InventoryScrollBox);
	PBGetterWidgetBlueprintVariable(UPBLobbyTabWidget, LobbyTab);

public:
	UPBLoadout_Inventory();
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void NotifyItemListReceived(const TArray<int32>& ItemList);

	/**
	* It is called to set the lobby tab's initial index at the first time when you are entering the inventory widget by selecting the item slot.
	*/
	void SetLobbyTabIndex(int32 Index);

protected:
	virtual void BindEvents() override;

public:
	/**
	* Event handlers
	*/

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget) override;

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey) override;

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused) override;

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget) override;
	
	/**
	* Lobby Tab
	*/

	UFUNCTION()
		void HandlePBLobbyTabChanged(int32 SlotIndex);



	
	
};
