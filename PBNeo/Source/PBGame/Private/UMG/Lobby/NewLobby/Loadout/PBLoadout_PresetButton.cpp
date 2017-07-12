// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_PresetButton.h"


PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLoadout_PresetButton, SameDepth_Focused_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLoadout_PresetButton, SameDepth_Normal_Ani);

UPBLoadout_PresetButton::UPBLoadout_PresetButton()
{
	PresetSlotIndex = 0;
}

void UPBLoadout_PresetButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPBLoadout_PresetButton::UpdateButtonLook()
{
	class UWidgetAnimation* Anim = nullptr;

	if (true == ContainUserFocus(GetOwningLocalPlayer(), this))
	{
		Anim = GetBPP_SameDepth_Focused_Ani();
	}
	else
	{
		Anim = GetBPP_SameDepth_Normal_Ani();
	}

	AnimToPlay = Anim;
}
