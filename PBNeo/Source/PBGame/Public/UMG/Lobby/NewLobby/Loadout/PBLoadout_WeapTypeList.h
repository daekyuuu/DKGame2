// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBLoadout_WeapTypeList.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_WeapTypeList : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()

public:

	PBGetterWidgetBlueprintVariable(UScrollBox, WeapTypeScrollBox);

	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeBtn, Type0);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeBtn, Type1);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeBtn, Type2);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeBtn, Type3);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTypeBtn, Type4);

public:
	UPBLoadout_WeapTypeList();

	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	virtual void NotifyDepthChanged(int32 NewDepth) override;
	virtual void NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget) override;

private:

	void InitTypeBtns();
	void InitTypeBtnArray();
	void InitTypeBtnTexts();
	void InitTypeBtnIds();



	// Remove the uninitialized widget from the panel.
	int32 ShrinkContentAccordingToTextArray(class UPanelWidget* Panel, const TArray<FText>& TextArray, int32 RemoveOffset);

public:

	void BindEvents();
	
	UFUNCTION()
	void HandlePBButtonReleased(UPBUserWidget* Widget);

	UFUNCTION()
	void HandlePBButtonCanceled(UPBUserWidget* Widget);

	UFUNCTION()
	void HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused);

	UFUNCTION()
	void HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey);


private:
	UPROPERTY()
	TArray<class UPBLoadout_WeapTypeBtn*> TypeBtns;

public:

	UPROPERTY()
	class UPBUserWidget* LastFocusedWidget;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	TArray<EWeaponType> TypeEnums;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	TArray<FText> TypeTexts;
	
	
};
