// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBHitCrosshairWidget.h"

void UPBHitCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnHitSuccess, UPBHitCrosshairWidget::NotifyHitSuccess);
	BindWidgetEvent(OnWarmupStarted, UPBHitCrosshairWidget::NotifyWarmupStarted);

}

void UPBHitCrosshairWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnHitSuccess);
	UnbindWidgetEvent(OnWarmupStarted);


	Super::NativeDestruct();
}

void UPBHitCrosshairWidget::NotifyHitSuccess_Implementation(bool bIsKilledByHit)
{
	// Implement in BP
}
