// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBHitIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBHitIndicatorWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UPBHitIndicatorWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void NotifyGotHit(float DamageTaken, const FVector& EnemyPosition);

protected:
	

	/** Calculate hit indicator Direction for HUD*/
	UFUNCTION(BlueprintCallable, Category = "HitIndicator")
	void CalculateHitIndicatorDirection();
	
	/** Show Hit Indicator HUD */
	UFUNCTION(BlueprintImplementableEvent, Category = "HitIndicator")
	void SpawnHitIndicator();


protected:

	float LastDamageTaken;
	FVector LastEnemyPosition;

	/** the direction of hit indicator */
	UPROPERTY(BlueprintReadOnly, Category = "HitIndicator")
		float HitIndicatorAngle;

	/** more damage, more intensity. */
	UPROPERTY(BlueprintReadOnly, Category = "HitIndicator")
		float HitIndicatorIntensity;
};
