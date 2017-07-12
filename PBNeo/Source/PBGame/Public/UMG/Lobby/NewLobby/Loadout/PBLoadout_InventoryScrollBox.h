// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLoadout_InventoryScrollBox.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_InventoryScrollBox : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UScrollBox, ScrollBox);

public:
	UPBLoadout_InventoryScrollBox();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

public:
	void NotifyItemListReceived(const TArray<int32>& ItemList);

private:

	virtual void BindEvents() override;

	void BuildScrollBoxContents(const TArray<int32>& ItemList);

	void NavigateUserFocusToTheFirstWidget();

public:

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget);

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey);

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused);

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget);

	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
		TSubclassOf<class UPBLoadout_ItemVector2> ItemVector2Class;
	
	
};
