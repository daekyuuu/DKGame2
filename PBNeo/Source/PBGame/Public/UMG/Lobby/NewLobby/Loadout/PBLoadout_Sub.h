// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_Sub.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_Sub : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()

public:
	PBGetterWidgetBlueprintVariable(UPBLoadout_ItemList, LoadoutItemList);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBItemSlotReleased, int32, ItemId);

public:
	FOnPBItemSlotReleased OnPBItemSlotReleased;


public:
	UPBLoadout_Sub();
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
