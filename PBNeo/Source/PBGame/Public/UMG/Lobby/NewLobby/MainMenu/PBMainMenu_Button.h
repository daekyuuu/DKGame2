// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Widgets/PBAnimBasedButton.h"
#include "PBMainMenu_Button.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMainMenu_Button : public UPBAnimBasedButton
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Focused_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Normal_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, OtherDepth_Focused_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, OtherDepth_Normal_Ani);

public:
	UPBMainMenu_Button();

public:
	virtual void NativeConstruct() override;
	virtual void UpdateButtonLook() override;

protected:
	bool AmILastFocusedButton();

public:
	UPROPERTY(EditAnywhere, Category = "Button")
	EPageType TargetPage;

	UPROPERTY(EditAnywhere, Category = "Button")
	int32 TargetTabIndex;


	
};
