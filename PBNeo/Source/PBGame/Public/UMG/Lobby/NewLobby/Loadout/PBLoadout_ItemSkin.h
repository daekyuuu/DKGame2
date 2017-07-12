// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Widgets/PBAnimBasedButton.h"
#include "PBLoadout_ItemSkin.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_ItemSkin : public UPBAnimBasedButton
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Focused_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Normal_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Click_Ani);


public:
	UPBLoadout_ItemSkin();

public:
	virtual void NativeConstruct() override;
	virtual void UpdateButtonLook() override;

	void ShowClickEffect();


private:

	
	
};
