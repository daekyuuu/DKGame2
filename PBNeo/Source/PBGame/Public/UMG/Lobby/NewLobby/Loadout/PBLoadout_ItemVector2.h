// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_ItemVector2.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_ItemVector2 : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UHorizontalBox, ItemHorizontalBox);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, Item0);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Item, Item1);


public:
	UPBLoadout_ItemVector2();
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;


public:
	void SetItemsId(int32 ItemId0, int32 ItemId1);

public:
	virtual void BindEvents() override;

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget);

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey);

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused);

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget);
	
	
};
