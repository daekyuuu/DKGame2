// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbyTabButton.h"

PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLobbyTabButton, SameDepth_Focused_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLobbyTabButton, SameDepth_Normal_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLobbyTabButton, OtherDepth_Focused_Ani);

UPBLobbyTabButton::UPBLobbyTabButton()
{
	bIsActivated = false;
}

void UPBLobbyTabButton::Activate(bool bActive)
{
	bIsActivated = bActive;
	UpdateButtonLook();
}

void UPBLobbyTabButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPBLobbyTabButton::UpdateButtonLook()
{
	UWidgetAnimation* Anim = nullptr;

	if (true == bIsActivated)
	{
		Anim = GetBPP_SameDepth_Focused_Ani();
	}
	else
	{
		Anim = GetBPP_SameDepth_Normal_Ani();
	}

	if (Anim)
	{
		AnimToPlay = Anim;
	}
}

