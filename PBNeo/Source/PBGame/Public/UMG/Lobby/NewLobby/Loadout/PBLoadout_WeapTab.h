// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBLoadout_WeapTab.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_WeapTab : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()

public:

	PBGetterWidgetBlueprintVariable(UScrollBox, WeapClipScrollBox);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapClipBtn, WeapClipMain);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapClipBtn, WeapClipSub);

	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapList, WeapList);

	PBGetterWidgetBlueprintVariable(UWidgetSwitcher, WeapTypeListSwitcher);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeList, WeapTypeMain);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeList, WeapTypeSub);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeapTypeFocusedEvent, int32, WeapType);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeapBtnReleasedEvent, int32, ItemId);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnWeapTypeFocusedEvent OnWeapTypeFocused;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnWeapBtnReleasedEvent OnWeapBtnReleased;


public:
	UPBLoadout_WeapTab();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

private:

	void BindEvents();
	void BindEventsOfWeapClip();
	void BindEventsOfWeapType();
	void BindEventsOfWeapList();

	virtual void NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget) override;


private:

	/* ------------------------------------------------------------------------------- */
	// Handle weapon clip Event
	/* ------------------------------------------------------------------------------- */
	UFUNCTION()
	void HandleWeapClipBtnReleased(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapClipBtnCanceled(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapClipGotNaviInput(UPBUserWidget* Widget, FKey PressedKey);

	UFUNCTION()
	void HandleWeapClipFocused(UPBUserWidget* Widget, bool bFocused);

	/* ------------------------------------------------------------------------------- */
	// Handle Weap Type Event
	/* ------------------------------------------------------------------------------- */

	UFUNCTION()
	void HandleWeapTypeCanceled(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapTypeReleased(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapTypeFocused(UPBUserWidget* Widget, bool bFocused);


	/* ------------------------------------------------------------------------------- */
	// Handle WeaponList Event
	/* ------------------------------------------------------------------------------- */

	UFUNCTION()
	void HandleWeapBtnCanceled(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapBtnReleased(UPBUserWidget* Widget);

	UFUNCTION()
	void HandleWeapBtnFocused(UPBUserWidget* Widget, bool bFocused);


private:
	void BroadcastDepthChanged(int32 NewDepth);

	void NavigateToNextClip(bool bInverseDir);
	void NavigateToWeapTypeBtn(class UPBLoadout_WeapClipBtn* CallClip);
	void NavigateToWeapListIfPossible();

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Loadout")
		void BuildPBWidgets(EWeaponType Type, const TArray<FPBItemBaseInfo>& ItemInfos);

private:

	/* ------------------------------------------------------------------------------- */
	// Variables for remember the last selection.
	/* ------------------------------------------------------------------------------- */
	
	 int32 const MainClipIndex = 0;
	 int32 const SubClipIndex = 1;
	 int32 CurrClipIndex;

	UPROPERTY()
	UPBUserWidget* LastFocusedClipBtn;

	UPROPERTY()
	UPBUserWidget* LastFocusedTypeBtn;

	UPROPERTY()
	class UPBLoadout_WeapTypeBtn* LastReleasedMainTypeBtn;

	UPROPERTY()
	class UPBLoadout_WeapTypeBtn* LastReleasedSubTypeBtn;

	UPROPERTY()
	UPBUserWidget* LastFocusedWidget;

	UPROPERTY()
	int32 CurrDepth;
	
};
