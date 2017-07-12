// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Widgets/PBAnimBasedButton.h"
#include "PBLoadout_PresetButton.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_PresetButton : public UPBAnimBasedButton
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Focused_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Normal_Ani);

public:
	UPBLoadout_PresetButton();
	virtual void NativeConstruct() override;
	virtual void UpdateButtonLook() override;


public:
	UPROPERTY(EditAnywhere, Category = "Loadout")
	int32 PresetSlotIndex;

	
};
