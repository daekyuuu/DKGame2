// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBHitIndicatorWidget.h"
#include "PBCharacter.h"



UPBHitIndicatorWidget::UPBHitIndicatorWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UPBHitIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPBHitIndicatorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBHitIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBHitIndicatorWidget::NotifyGotHit(float DamageTaken, const FVector& EnemyPosition)
{
	LastDamageTaken = DamageTaken;
	LastEnemyPosition = EnemyPosition;

	CalculateHitIndicatorDirection();
	SpawnHitIndicator();
}

void UPBHitIndicatorWidget::CalculateHitIndicatorDirection()
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
	HitIndicatorIntensity = FMath::Clamp(LastDamageTaken / MaxHealth, 0.0f, 1.0f);


	FVector EnemyPos = LastEnemyPosition;
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
	* Decide Rotation direction by weather (U dot (V cross D) > 0) or not.
	*/
	FVector AxisOfRotation = FVector::CrossProduct(HitDir, PlayerForward);
	AxisOfRotation.Normalize();
	float DirectionOfRotation = FVector::DotProduct(AxisOfRotation, PlayerUp);

	HitIndicatorAngle = (DirectionOfRotation < 0) ? RotationAngle : -RotationAngle;

	// Indicator direction is opposite of hit direction.
	HitIndicatorAngle += 180.0f;
}
