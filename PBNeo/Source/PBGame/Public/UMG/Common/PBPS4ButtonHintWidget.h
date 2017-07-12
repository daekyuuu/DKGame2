// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBPS4ButtonHintWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBPS4ButtonHintWidget : public UPBSubWidget
{
	GENERATED_BODY()

public:
	PBGetterWidgetBlueprintVariable(UImage, ButtonImage);
	PBGetterWidgetBlueprintVariable(UTextBlock, ButtonHint);

public:

	UPBPS4ButtonHintWidget();

	virtual void DesignTimeConstruct_Implementation();

public:
	UPROPERTY(EditAnywhere, Category = "PS4ButtonHint")
	class UTexture* ButtonTexture;

	UPROPERTY(EditAnywhere, Category = "PS4ButtonHint")
	FText HintText;
	
	
	
};
