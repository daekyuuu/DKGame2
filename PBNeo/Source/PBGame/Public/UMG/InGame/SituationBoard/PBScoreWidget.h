// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBScoreWidget : public UPBSubWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "ScoreBoard")
	class UPBPlayerInfoAccessComponent* PlayerInfoAccessComp;
	
	
	
};
