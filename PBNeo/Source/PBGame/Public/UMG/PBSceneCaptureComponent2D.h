// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneCaptureComponent2D.h"
#include "PBSceneCaptureComponent2D.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
public:
	virtual void OnRegister() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	
	
};
