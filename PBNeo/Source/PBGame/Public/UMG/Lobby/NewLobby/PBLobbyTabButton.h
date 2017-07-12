// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Widgets/PBAnimBasedButton.h"
#include "PBLobbyTabButton.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLobbyTabButton : public UPBAnimBasedButton
{
	GENERATED_BODY()

public:

	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Focused_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, SameDepth_Normal_Ani);
	PBGetterWidgetBlueprintProperty(UWidgetAnimation, OtherDepth_Focused_Ani);

public:
	UPBLobbyTabButton();

	void Activate(bool bActive);


public:
	virtual void NativeConstruct() override;
	virtual void UpdateButtonLook() override;

private:
	bool bIsActivated;
	
};
