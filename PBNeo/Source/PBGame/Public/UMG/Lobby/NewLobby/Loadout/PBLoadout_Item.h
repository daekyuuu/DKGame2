// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_Item.generated.h"



/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_Item : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UImage, ItemImage);
	PBGetterWidgetBlueprintVariable(UImage, GradeImage);
	PBGetterWidgetBlueprintVariable(UTextBlock, ItemNameText);
	PBGetterWidgetBlueprintVariable(UTextBlock, RemainingTimeText);
	PBGetterWidgetBlueprintVariable(UPBLoadout_ItemSkin, Loadout_ItemSkin);


public:
	
	UPBLoadout_Item();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void SetItemId(int32 InItemId);
	int32 GetItemId() const { return ItemId; }

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

private:

	UPROPERTY()
	int32 ItemId;
	
	
};
