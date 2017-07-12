// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMainWidget.h"

void UPBMainWidget::Show(int32 ZOrder)
{
	if (!IsInViewport())
	{
		AddToPlayerScreen();
	}
}

void UPBMainWidget::Hide()
{
	RemoveFromViewport();
}

bool UPBMainWidget::IsOpened()
{
	return IsInViewport();
}
