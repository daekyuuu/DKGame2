// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_ItemList.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_ItemList : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UCanvasPanel, ItemCanvas);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, LoadoutItemMain);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, LoadoutItemSub);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, LoadoutItemGrenade);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, LoadoutItemSpecial);
public:
	UPBLoadout_ItemList();

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	
public:
	void NotifyItemListReceived(const TArray<int32>& ItemList);

public:

	virtual void BindEvents() override;

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget) override;

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey) override;

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused) override;

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget) override;

	
};
