// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBUserWidget.h"
#include "PBMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMainWidget : public UPBUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	virtual void Show(int32 ZOrder = 0);
	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	virtual void Hide();

	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	bool IsOpened();
};
