// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SlateDelegates.h"
#include "UMG/Dialog/PBDialogWidget.h"
#include "PBDialogNoticeWidget.generated.h"

UCLASS()
class PBGAME_API UPBDialogNoticeWidget : public UPBDialogWidget
{
	GENERATED_BODY()
	
public: // For CPP
	void Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg, 
		FText AcknowledgeTextArg, FOnClicked OnAcknowledgedClickedArg);

public: // For BP
	UFUNCTION(BlueprintCallable, Category = Dialog)
	FText GetAcknowledgeText();

	UFUNCTION(BlueprintCallable, Category = Dialog)
	void ClickAcknowledge();

protected:
	FOnClicked OnAcknowledgeClicked;

	FText AcknowledgeText;
	
};
