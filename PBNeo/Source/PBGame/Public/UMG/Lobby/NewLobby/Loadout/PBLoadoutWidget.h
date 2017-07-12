// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/PBLobbyWidget.h"
#include "PBLoadoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadoutWidget : public UPBLobbyWidget
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UPBLoadout_Main, Loadout_Main);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Sub, Loadout_Sub);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Inventory, Loadout_WeapInventory);
	PBGetterWidgetBlueprintVariable(UWidgetSwitcher, Loadout_Switcher);

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;


protected:
	void BindEvents();
	void BindEventsOfLoadout_Main();
	void BindEventsOfLoadout_Sub();
	void BindEventsOfLoadout_Inventory();


	/* ------------------------------------------------------------------------------- */
	// Handle events came from children
	/* ------------------------------------------------------------------------------- */

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget);

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey);

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused);

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget);

	/**
	* Main
	*/

	UFUNCTION()
		void HandlePBPresetButtonFocused(int32 SlotIndex);

	UFUNCTION()
		void HandlePBPresetButtonReleased(int32 SlotIndex);

	/**
	* Sub
	*/

	UFUNCTION()
		void HandlePBItemSlotReleased(int32 ItemId);

	/**
	* Inventory
	*/
	UFUNCTION()
		void HandleInventoryTabChanged(int32 SlotIndex);
	
};
