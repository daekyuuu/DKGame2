// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBItemInfo.h"
#include "PBLoadout_WeapVector4.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_WeapVector4 : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
public:

	PBGetterWidgetBlueprintVariable(UPBLoadout_Weap, Column0);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Weap, Column1);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Weap, Column2);
	PBGetterWidgetBlueprintVariable(UPBLoadout_Weap, Column3);

	PBGetterWidgetBlueprintVariable(UHorizontalBox, Row0);

	
public:

	UPBLoadout_WeapVector4();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BuildPBWidgets(const TArray<FPBItemBaseInfo>& Infos);

	void CheckTheTargetAndUncheckOthers(class UPBUserWidget* TargetWidget);

	class UPBLoadout_Weap* FindWeapFromId(int32 Id);

	int32 GetFocusedColumnIndex();

	void NavigateFocusAt(int32 Index);

	virtual void NotifyDepthChanged(int32 NewDepth) override;
	virtual void NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget) override;

private:
	void BuildPropertyArray();

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
	TArray<class UPBLoadout_Weap*> Columns;

};
