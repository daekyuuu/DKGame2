// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPS4ButtonHintWidget.h"
#include "PBUMGUtils.h"
#include "TextBlock.h"
#include "Image.h"

PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBPS4ButtonHintWidget, ButtonImage);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBPS4ButtonHintWidget, ButtonHint);

UPBPS4ButtonHintWidget::UPBPS4ButtonHintWidget()
{
	HintText = FText::FromString("DefultText");
}

void UPBPS4ButtonHintWidget::DesignTimeConstruct_Implementation()
{
	if (GetBPV_ButtonImage() && GetBPV_ButtonHint())
	{
		UPBUMGUtils::SetOnlyBrushImage(GetBPV_ButtonImage(), ButtonTexture);
		GetBPV_ButtonHint()->SetText(HintText);
	}
}

