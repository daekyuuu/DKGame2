// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBSubWidget_HavePBButton.h"
#include "PBItemInfo.h"
#include "PBLoadout_SoldierTab.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_SoldierTab : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UScrollBox, ScrollBoxRed);
	PBGetterWidgetBlueprintVariable(UScrollBox, ScrollBoxBlue);


public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void BroadcastDepthChanged(int32 NewDepth);


public:
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	void BuildPBWidgets(EPBTeamType Type, const TArray<FPBItemBaseInfo>& ItemInfos);

public:
	void BindEvents();

	UFUNCTION()
	virtual void HandlePBButtonReleased(class UPBUserWidget* Widget) override;

	UFUNCTION()
	virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget) override;

	UFUNCTION()
	virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused) override;

	UFUNCTION()
	virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey) override;

private:

	void NavigateToNext(bool bInverseDir);
	void NavigateToUp(bool bInverseDir);


	TArray<class UPBLoadout_SoldierBtn*> GetSoldierBtnsInScrollBox(class UScrollBox* TargetScrollBox);

	void AddOneSoldierBtn(class UScrollBox* TargetScrollBox, const FPBItemBaseInfo& ItemInfo);
	void AddOneSpacer(class UScrollBox* TargetScrollBox);

	void CheckTheTargetAndUncheckOthers(class UScrollBox* TargetScrollBox, class UPBUserWidget* TargetWidget);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<class UPBLoadout_SoldierBtn> SoldierBtnWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<class UPBSubWidget> PBSpacerWidgetClass;

private:
	int32 const OffsetForFocus = 1;
	int32 const OffsetForScroll = 2;
	float const ScrollOffsetForOneStep = 270.0f;

private:
	UPROPERTY()
		class UPBUserWidget* LastFocusedWidget;


};
