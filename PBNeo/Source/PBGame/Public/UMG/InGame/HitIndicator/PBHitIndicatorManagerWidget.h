// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBHitIndicatorManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBHitIndicatorManagerWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void NotifyGotHit(float DamageTaken, const FVector& EnemyPosition, bool bIsDamagedByOwnself);

protected:

	// This widget have multiple indicator widget for multiple effect.
	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	TArray<UPBSubWidget*> IndicatorQueue;

	// current indicator index of queue.
	int32 CurrIndicatorIndex;
	
};
