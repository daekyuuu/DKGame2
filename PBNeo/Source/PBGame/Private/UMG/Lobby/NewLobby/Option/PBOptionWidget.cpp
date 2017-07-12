// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBOptionWidget.h"
#include "PBOptionPage.h"



void UPBOptionWidget::OnGammaSelectionUpdated(int32 Index)
{
	UPBOptionPage* p = GetOwningPage<UPBOptionPage>();
	if (p)
	{
		p->SetGamma(Index);
	}
}

void UPBOptionWidget::OnAimSensitivitySelectionUpdated(int32 Index)
{
	UPBOptionPage* p = GetOwningPage<UPBOptionPage>();
	if (p)
	{
		p->SetAimSensitivity(Index);
	}
}

void UPBOptionWidget::OnYAxisInversionUpdated(bool bIsInverse)
{
	UPBOptionPage* p = GetOwningPage<UPBOptionPage>();
	if (p)
	{
		p->SetInvertYAxis(bIsInverse);
	}
}

void UPBOptionWidget::OnApplyChangeClicked()
{
	UPBOptionPage* p = GetOwningPage<UPBOptionPage>();
	if (p)
	{
		p->DoApplyChanges();
	}
}


int UPBOptionWidget::GetAimSensitivityIndex(float Value)
{
	int32 SensivitityIndex = FMath::RoundToInt((Value - 0.1f) * 100.0f);
	return FMath::Clamp(SensivitityIndex, 0, 100);
}

int UPBOptionWidget::GetGammaIndex(float Value)
{
	int32 GammaIndex = FMath::TruncToInt(((Value - 2.2f) / 2.0f) * 20.f) + 10;
	return FMath::Clamp(GammaIndex, 0, 20);
}


TArray<FText> UPBOptionWidget::GetGammaList()
{
	TArray<FText> GammaList;
	for (int32 i = -10; i < 11; i++)
	{
		GammaList.Add(FText::AsNumber(i));
	}
	return GammaList;
}

TArray<FText> UPBOptionWidget::GetAimSensitivityList()
{
	TArray<FText> SensitivityList;
	for (int32 i = 0; i < 101; i++)
	{
		SensitivityList.Add(FText::AsNumber(i));
	}
	return SensitivityList;
}


