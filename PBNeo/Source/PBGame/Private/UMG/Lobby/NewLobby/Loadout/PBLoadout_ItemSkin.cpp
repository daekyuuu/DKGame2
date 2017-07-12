// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_ItemSkin.h"

PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLoadout_ItemSkin, SameDepth_Focused_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLoadout_ItemSkin, SameDepth_Normal_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBLoadout_ItemSkin, SameDepth_Click_Ani);


UPBLoadout_ItemSkin::UPBLoadout_ItemSkin()
{

}

void UPBLoadout_ItemSkin::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateButtonLook();
}

void UPBLoadout_ItemSkin::UpdateButtonLook()
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

void UPBLoadout_ItemSkin::ShowClickEffect()
{
	auto Anim = GetBPP_SameDepth_Click_Ani();
	PlayAnimation(Anim);
}
