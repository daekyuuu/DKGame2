// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBHitCrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBHitCrosshairWidget : public UPBSubWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "HitCrosshair")
	void NotifyWarmupStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "HitCrosshair")
	void NotifyHitSuccess(bool bIsKilledByHit);	
};
