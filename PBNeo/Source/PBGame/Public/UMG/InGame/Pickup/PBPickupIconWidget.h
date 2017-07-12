// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBPickupIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBPickupIconWidget : public UPBSubWidget
{
	GENERATED_BODY()

public:
	PBGetterWidgetBlueprintVariable(UTextBlock, ItemName);
	PBGetterWidgetBlueprintVariable(UBorder, CanPickupUI);
	PBGetterWidgetBlueprintVariable(UProgressBar, Gauge);

public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void InitWidget(class APBPickup* InPickup);

	void ShowPickupReadyUI(bool bVisible);
	void StartGaugeFilling(float TimerDuration);
	void StopGaugeFilling();

private:
	void UpdateGauge(float DeltaTime);
	void InitName();

protected:

	// Icon widget will show the information of this Pickup.
	UPROPERTY()
	TWeakObjectPtr<class APBPickup> Pickup;
	
	// The duration of pickup gauge progress
	UPROPERTY()
		float Duration;

	// Current time of gauge tick animation
	UPROPERTY()
		float CurrTime;

	UPROPERTY()
		bool bPlayingGaugeFill;

	// [0, 1]:  CurrTime/Duration
	UPROPERTY()
		float GaugePercentage;
	
	
};
