// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBHitEffectPlayer.h"



void UPBHitEffectPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnHit, UPBHitEffectPlayerWidget::NotifyGotHit);

}

void UPBHitEffectPlayerWidget::NativeDestruct()
{

	UnbindWidgetEvent(OnHit);

	Super::NativeDestruct();
}

void UPBHitEffectPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBHitEffectPlayerWidget::NotifyGotHit(float DamageTaken, const FVector& EnemyPosition, bool bIsDamagedByOwnself)
{
	if (false == bIsDamagedByOwnself)
		CalculateHitDirection(DamageTaken, EnemyPosition);

	EHitEffectType Type = bIsDamagedByOwnself ? EHitEffectType::All : CalculateHitEffectType();
	PlayHitEffect(Type);
}

void UPBHitEffectPlayerWidget::CalculateHitDirection(float DamageTaken, const FVector& EnemyPosition)
{
	APBCharacter* OwningCharacter = GetOwningPlayerPawn() ? Cast<APBCharacter>(GetOwningPlayerPawn()) : nullptr;
	if (false == IsSafe(OwningCharacter))
	{
		return;
	}

	auto OwningController = GetOwningPlayer();
	if (false == IsSafe(OwningController))
	{
		return;
	}

	// Set Hit indicator intensity by Damage;
	const float MaxHealth = OwningCharacter->GetMaxHealth();
	HitIndicatorIntensity = FMath::Clamp(DamageTaken / MaxHealth, 0.0f, 1.0f);


	FVector EnemyPos = EnemyPosition;
	FVector PlayerPos = OwningCharacter->GetActorLocation();

	// Assume that HitDirection is PlayerPosition - EnemyPosition.
	FVector HitDir = PlayerPos - EnemyPos;
	HitDir = HitDir.GetSafeNormal();

	// Use forward direction as Controller Desired diection.
	FVector PlayerForward = OwningController->GetDesiredRotation().Vector();
	FVector PlayerUp = OwningController->GetActorUpVector();

	// calculate rotation angle.
	float RotationAngle = FVector::DotProduct(HitDir, PlayerForward);
	RotationAngle = FMath::Acos(RotationAngle);
	RotationAngle = FMath::RadiansToDegrees(RotationAngle);

	/**
	* U : PlayerUp, V : PlayerForward, D : HitDirection.
	* Decide Rotation direction by whether (U dot (V cross D) > 0) or not.
	*/
	FVector AxisOfRotation = FVector::CrossProduct(HitDir, PlayerForward);
	AxisOfRotation.Normalize();
	float DirectionOfRotation = FVector::DotProduct(AxisOfRotation, PlayerUp);

	HitIndicatorAngle = (DirectionOfRotation < 0) ? RotationAngle : -RotationAngle;

	// Indicator direction is opposite of hit direction.
	HitIndicatorAngle += 180.0f;
}

EHitEffectType UPBHitEffectPlayerWidget::CalculateHitEffectType() const
{

	if (HitIndicatorIntensity >= 0.5f)
	{
		return EHitEffectType::All;
	}

	if(HitIndicatorAngle >= 0.0f && HitIndicatorAngle < 45.0f)
	{
		return EHitEffectType::Up;
	}
	else if (HitIndicatorAngle >= 45.0f && HitIndicatorAngle < 135.0f)
	{
		return EHitEffectType::Right;
	}
	else if (HitIndicatorAngle >= 135.0f && HitIndicatorAngle < 225.0f)
	{
		return EHitEffectType::Down;
	}
	else if (HitIndicatorAngle >= 225.0f && HitIndicatorAngle < 315.0f)
	{
		return EHitEffectType::Left;
	}
	else
	{
		return EHitEffectType::Up;
	}
}
