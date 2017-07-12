// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBCrossHairImageWidget.generated.h"

/**
 *  The interface class for cross hair images owned by UPBCrossHairWidget
 */
UCLASS()
class PBGAME_API UPBCrossHairImageWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = CrossHair)
	void SetSize(float Value);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = CrossHair)
	void SetAimingAtEnemy(bool LockedEnemy);
		
};
