// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBombMissionTitleMsg.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBombMissionTitleMsg : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Mission Title")
	void NotifyMissionRenewed();

	UFUNCTION(BlueprintNativeEvent, Category = "Mission Title")
	void NotifyBombPlanted();

};
