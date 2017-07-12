// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBSubWidget_HavePBButton.h"
#include "Components/PanelWidget.h"
#include "WidgetSwitcher.h"
#include "PBSwitcherBasedButton.generated.h"



/**
 * 
 */
UCLASS()
class PBGAME_API UPBSwitcherBasedButton : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:

	PBGetterWidgetBlueprintVariable(UWidgetSwitcher, ButtonStateSwitcher);
	PBGetterWidgetBlueprintVariable(UWidget, SameDepth_Normal);
	PBGetterWidgetBlueprintVariable(UWidget, SameDepth_Focused);
	PBGetterWidgetBlueprintVariable(UWidget, OtherDepth_Normal);
	PBGetterWidgetBlueprintVariable(UWidget, OtherDepth_Focused);

	PBGetterWidgetBlueprintVariable(UWidget, CheckedWidget);

public:
	UPBSwitcherBasedButton();

public:

	// I've override this to prevent ue4 default widget navigation system.
	virtual void OnWidgetRebuilt() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// Update the button look according to current state.
	virtual void UpdateButtonLook();

public:
	// It should be called before have got the focus.
	virtual void NotifyDepthChanged(int32 NewDepth) override;

	virtual void SetCheck(bool InChecked);

	virtual bool IsChecked() const;

	virtual void SetFocusedBeforeDepthChange(bool bRes);

protected:

	// Is this widget In the same depth with the current focused widget? 
	bool bInSameDepth;

	// Is this widget checked?
	bool bChecked;

	// Is this widget focused before the cancel event?
	bool bFocusedBeforeDepthChange;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Depth")
	int32 MyDepth;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	USoundCue* PressedSound;



};
