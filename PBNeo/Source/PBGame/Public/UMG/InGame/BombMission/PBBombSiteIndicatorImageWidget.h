// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBombSiteIndicatorImageWidget.generated.h"

class APBCharacter;

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBombSiteIndicatorImageWidget : public UPBSubWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintProperty(UWidgetAnimation, TargetOn);

	PBGetterWidgetBlueprintVariable(UImage, DirectionIndicator);
	PBGetterWidgetBlueprintVariable(UTextBlock, PointName);
	PBGetterWidgetBlueprintVariable(UBorder, DistanceBorder);
	PBGetterWidgetBlueprintVariable(UTextBlock, DistanceText);

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "BombSite")
	void SetSize(float Value);
};
