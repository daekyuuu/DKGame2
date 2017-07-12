// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBWaitDialogWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBWaitDialogWidget : public UPBMainWidget
{
	GENERATED_BODY()
	
	/* ------------------------------------------------------------------------------------ */
	// General information every Dialog should have

public: // For BP
	UFUNCTION(BlueprintCallable, Category = Dialog)
	FText GetMessageText();

	void Init(FText MessageTextArg);

protected:
	FText MessageText;	
};
