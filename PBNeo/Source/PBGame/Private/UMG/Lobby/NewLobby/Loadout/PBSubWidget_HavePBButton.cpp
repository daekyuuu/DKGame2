// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSubWidget_HavePBButton.h"

UPBSubWidget_HavePBButton::UPBSubWidget_HavePBButton()
{
	bIsFocusable = true;
}

void UPBSubWidget_HavePBButton::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBSubWidget_HavePBButton::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UPBSubWidget_HavePBButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	//return TransferUserFocusThroughReply(InFocusEvent, E, TargetXXX).NativeReply;

	// ...

	return SuperReply;
}

void UPBSubWidget_HavePBButton::BindEvents()
{
	// ...
}
