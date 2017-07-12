// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBombSiteIndicatorImageWidget.h"

PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBBombSiteIndicatorImageWidget, TargetOn);

PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBBombSiteIndicatorImageWidget, DirectionIndicator);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBBombSiteIndicatorImageWidget, PointName);
PBGetterWidgetBlueprintVariable_Implementation(UBorder, UPBBombSiteIndicatorImageWidget, DistanceBorder);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBBombSiteIndicatorImageWidget, DistanceText);

void UPBBombSiteIndicatorImageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// getting cache on
	{
		GetBPV_DirectionIndicator();
		GetBPV_PointName();
		GetBPV_DistanceBorder();
		GetBPV_DistanceText();
	}
}

void UPBBombSiteIndicatorImageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBBombSiteIndicatorImageWidget::NativeDestruct()
{
	Super::NativeDestruct();
}