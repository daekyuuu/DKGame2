// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBDialogNoticeWidget.h"

void UPBDialogNoticeWidget::Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg, 
	FText AcknowledgeTextArg, FOnClicked OnAcknowledgedClickedArg)
{
	Super::Init(PlayerOwnerArg, MessageTextArg);

	AcknowledgeText = AcknowledgeTextArg;
	OnAcknowledgeClicked = OnAcknowledgedClickedArg;
}

FText UPBDialogNoticeWidget::GetAcknowledgeText()
{
	return AcknowledgeText;
}

void UPBDialogNoticeWidget::ClickAcknowledge()
{
	if (OnAcknowledgeClicked.IsBound())
	{
		OnAcknowledgeClicked.Execute();
	}
}