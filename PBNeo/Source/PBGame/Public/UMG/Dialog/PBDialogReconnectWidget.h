// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SlateDelegates.h"
#include "UMG/Dialog/PBDialogWidget.h"
#include "PBDialogReconnectWidget.generated.h"

UCLASS()
class PBGAME_API UPBDialogReconnectWidget : public UPBDialogWidget
{
	GENERATED_BODY()

public: // For CPP
	void Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg, 
		FOnClicked OnReconnectedClickArg);

protected:
	FOnClicked OnReconnectedClick;

	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

};
