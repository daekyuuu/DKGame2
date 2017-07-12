// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SlateDelegates.h"
#include "UMG/Dialog/PBDialogWidget.h"
#include "PBDialogConfirmWidget.generated.h"

UCLASS()
class PBGAME_API UPBDialogConfirmWidget : public UPBDialogWidget
{
	GENERATED_BODY()
	
public: // For CPP
	void Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg,
		FText ConfirmTextArg, FOnClicked OnConfirmClickedArg, 
		FText CancelTextArg, FOnClicked OnCancelClickedArg);

public: // For BP
	UFUNCTION(BlueprintCallable, Category = Dialog)
	FText GetConfirmText();

	UFUNCTION(BlueprintCallable, Category = Dialog)
	void ClickConfirm();

	UFUNCTION(BlueprintCallable, Category = Dialog)
	FText GetCancelText();

	UFUNCTION(BlueprintCallable, Category = Dialog)
	void ClickCancel();

protected:
	FText ConfirmText;
	FOnClicked OnConfirmClicked;

	FText CancelText;
	FOnClicked OnCancelClicked;
};
