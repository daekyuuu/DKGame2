// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PBMiniMapCanvas.generated.h"

UCLASS()
class PBGAME_API APBMiniMapCanvas : public AActor
{
	GENERATED_BODY()
	
public:	

	APBMiniMapCanvas();

	FVector2D GetExtent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MiniMap)
	UTexture2D* MiniMapTexture;

protected:

	UPROPERTY()
	class UBoxComponent* MiniMapExtent;
	
};
