// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBSubWidget_HavePBButton.h"
#include "PBLoadout_Main.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_Main : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UCanvasPanel, PresetButtonCanvas);
	PBGetterWidgetBlueprintVariable(UPBLoadout_ItemList, LoadoutItemList);


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBPresetButtonFocused, int32, SlotIndex);
	FOnPBPresetButtonFocused OnPBPresetButtonFocused;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBPresetButtonReleased, int32, SlotIndex);
	FOnPBPresetButtonReleased OnPBPresetButtonReleased;

public:
	void NotifyItemListReceived(const TArray<int32>& ItemList);


public:
	UPBLoadout_Main();
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;


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

	class UPBUserWidget* GetFirstButton();

	
	
	
};
