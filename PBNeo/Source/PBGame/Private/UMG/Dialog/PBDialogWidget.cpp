// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBDialogWidget.h"

void UPBDialogWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	// For dialogue widgets, we keep the widget alive across travel
	// Take care to not have reference to uobject other than uviewportclient or ugameinstance

	// WARNING:
	// This override may crash the game, although we are assuming it is fine
}

ULocalPlayer* UPBDialogWidget::GetTargetLocalPlayer()
{
	return OwningLocalPlayer;
}

FText UPBDialogWidget::GetMessageText()
{
	return MessageText;
}

void UPBDialogWidget::Init(ULocalPlayer* OwningLocalPlayerArg, FText MessageTextArg)
{
	bIsFocusable = true;
	OwningLocalPlayer = OwningLocalPlayerArg;
	MessageText = MessageTextArg;
}
