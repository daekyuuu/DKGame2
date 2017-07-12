// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBHitEffectPlayer.generated.h"


UENUM(BlueprintType)
enum class EHitEffectType : uint8
{
	Up,
	Down,
	Left,
	Right,
	All,
};


/**
 * 
 */
UCLASS()
class PBGAME_API UPBHitEffectPlayerWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void NotifyGotHit(float DamageTaken, const FVector& EnemyPosition, bool bIsDamagedByOwnself);

	UFUNCTION(BlueprintImplementableEvent, Category = "HitEffect")
	void PlayHitEffect(EHitEffectType EffectType);

	
private:
	void CalculateHitDirection(float DamageTaken, const FVector& EnemyPosition);

	EHitEffectType CalculateHitEffectType() const;

protected:
	/** the direction of hit indicator */
		float HitIndicatorAngle;

	/** more damage, more intensity. [0~1] */
		float HitIndicatorIntensity;
	
};
