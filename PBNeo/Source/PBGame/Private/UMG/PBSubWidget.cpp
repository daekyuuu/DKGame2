// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSubWidget.h"

void UPBSubWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UPBSubWidget::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

bool UPBSubWidget::IsOpened()
{
	return IsVisible();
}


void UPBSubWidget::ToggleWidget()
{
	if (IsOpened())
	{
		Hide();
	}
	else
	{
		Show();
	}
}

