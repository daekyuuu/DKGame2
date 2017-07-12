// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBDialogConfirmWidget.h"

void UPBDialogConfirmWidget::Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg, 
	FText ConfirmTextArg, FOnClicked OnConfirmClickedArg, 
	FText CancelTextArg, FOnClicked OnCancelClickedArg)
{
	Super::Init(PlayerOwnerArg, MessageTextArg);

	ConfirmText = ConfirmTextArg;
	OnConfirmClicked = OnConfirmClickedArg;

	CancelText = CancelTextArg;
	OnCancelClicked = OnCancelClickedArg;
}

FText UPBDialogConfirmWidget::GetConfirmText()
{
	return ConfirmText;
}

void UPBDialogConfirmWidget::ClickConfirm()
{
	if (OnConfirmClicked.IsBound())
	{
		OnConfirmClicked.Execute();
	}
}

FText UPBDialogConfirmWidget::GetCancelText()
{
	return ConfirmText;
}

void UPBDialogConfirmWidget::ClickCancel()
{
	if (OnCancelClicked.IsBound())
	{
		OnCancelClicked.Execute();
	}
}
