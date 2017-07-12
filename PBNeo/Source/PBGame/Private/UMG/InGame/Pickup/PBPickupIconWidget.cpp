// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupIconWidget.h"
#include "PBPickup.h"
#include "TextBlock.h"
#include "PBPickupWeapon.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBPickupIconWidget, ItemName);
PBGetterWidgetBlueprintVariable_Implementation(UBorder, UPBPickupIconWidget, CanPickupUI);
PBGetterWidgetBlueprintVariable_Implementation(UProgressBar, UPBPickupIconWidget, Gauge);


void UPBPickupIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateGauge(InDeltaTime);
}

void UPBPickupIconWidget::InitWidget(class APBPickup* InPickup)
{
	Pickup = InPickup;

	InitName();
	
}

void UPBPickupIconWidget::ShowPickupReadyUI(bool bVisible)
{
	if (bVisible)
	{
		GetBPV_CanPickupUI()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GetBPV_CanPickupUI()->SetVisibility(ESlateVisibility::Hidden);

	}
}

void UPBPickupIconWidget::StartGaugeFilling(float TimerDuration)
{
	bPlayingGaugeFill = true;
	CurrTime = 0.0f;
	Duration = TimerDuration;
	GaugePercentage = 0.0f;
	GetBPV_Gauge()->SetPercent(GaugePercentage);
}

void UPBPickupIconWidget::StopGaugeFilling()
{
	bPlayingGaugeFill = false;
	GetBPV_Gauge()->SetPercent(0.0f);
}

void UPBPickupIconWidget::UpdateGauge(float DeltaTime)
{
	if (bPlayingGaugeFill)
	{
		CurrTime += DeltaTime;

		GaugePercentage = CurrTime / Duration;
		GetBPV_Gauge()->SetPercent(GaugePercentage);

		if (GaugePercentage >= 1.0f)
		{
			StopGaugeFilling();
		}
	
	}
}

void UPBPickupIconWidget::InitName()
{
	if (Pickup.IsValid())
	{
		auto PickupWeap = Cast<APBPickupWeapon>(Pickup.Get());
		if (PickupWeap)
		{
			GetBPV_ItemName()->SetText(FText::FromString(PickupWeap->GetPickupName()));
		}
		else
		{
			FString Str = Pickup->GetName();
			auto Index = Str.Find("_");
			if (Index != INDEX_NONE)
			{
				Str.RemoveAt(0, Index + 1);
			}

			GetBPV_ItemName()->SetText(FText::FromString(Str));
		}
	}

}
