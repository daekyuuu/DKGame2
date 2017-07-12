// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWaitDialogWidget.h"

FText UPBWaitDialogWidget::GetMessageText()
{
	return MessageText;
}

void UPBWaitDialogWidget::Init(FText MessageTextArg)
{
	bIsFocusable = true;
	MessageText = MessageTextArg;
}
