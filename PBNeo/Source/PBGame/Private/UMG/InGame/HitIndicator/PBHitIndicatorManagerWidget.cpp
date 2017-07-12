// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBHitIndicatorWidget.h"
#include "PBHitIndicatorManagerWidget.h"




void UPBHitIndicatorManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrIndicatorIndex = 0;

	BindWidgetEvent(OnHit, UPBHitIndicatorManagerWidget::NotifyGotHit);
}

void UPBHitIndicatorManagerWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnHit);

	Super::NativeDestruct();

}

void UPBHitIndicatorManagerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBHitIndicatorManagerWidget::NotifyGotHit(float DamageTaken, const FVector& EnemyPosition, bool bIsDamagedByOwnself)
{
	if (CurrIndicatorIndex < IndicatorQueue.Num())
	{
		UPBHitIndicatorWidget* IndicatorWidget = Cast<UPBHitIndicatorWidget>(IndicatorQueue[CurrIndicatorIndex]);
		if (IndicatorWidget)
		{
			IndicatorWidget->NotifyGotHit(DamageTaken, EnemyPosition);
			CurrIndicatorIndex = (CurrIndicatorIndex + 1) % IndicatorQueue.Num();
		}
	}
}
