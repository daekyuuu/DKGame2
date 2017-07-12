// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_Sniper.h"


APBWeap_Sniper::APBWeap_Sniper()
{
	WeaponType = EWeaponType::SR;
	PostFiringZoomOutDelay = -1.0f;
}

void APBWeap_Sniper::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateZooming();
}

void APBWeap_Sniper::OnUnEquip()
{
	Super::OnUnEquip();

	if (PostFiringZoomOutDelay >= 0.f)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_PostFiringZoomOut);
	}
}

void APBWeap_Sniper::PostFiring(EWeaponPoint ePoint)
{
	Super::PostFiring(ePoint);

	//if (MyPawn && MyPawn->IsFirstPersonView())
	if (GetViewerIsFirstPerson())
	{
		if (PostFiringZoomOutDelay >= 0.f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_PostFiringZoomOut, this, &APBWeap_Sniper::PostFiringZoomOut, PostFiringZoomOutDelay, false);
		}
	}
}

void APBWeap_Sniper::PostFiringZoomOut()
{
	if (MyPawn && MyPawn->IsZooming())
	{
		MyPawn->SetZooming(false);
	}
}

void APBWeap_Sniper::OnFireFinished()
{
	FiringSync.BurstCounter = 0;
	bRefiring = false;
	bPendingFiring = false;
	//bPostFiring = false;		//PostFiringEnd 에서만 한다.
	GetWorldTimerManager().ClearTimer(TimerHandle_ReFiring);
}

bool APBWeap_Sniper::CanZooming() const
{
	if (!Super::CanZooming())
	{
		return false;
	}
	return !bPostFiring;
}

void APBWeap_Sniper::UpdateZooming()
{
	if (MyPawn && IsEquipped())
	{
		if (MyPawn->IsWantsToZooming() && !IsZooming() && !bPostFiring)
		{
			MyPawn->OnZoomIn();
		}
	}
}

void APBWeap_Sniper::SetZooming(bool bZoom)
{
	if (MyPawn == nullptr)
		return;

	if (bZoom)
	{
		// Hide
		//if (MyPawn->IsFirstPersonView())
		if (GetViewerIsFirstPerson())
		{
			UpdateMeshVisibilitys(false);
			MyPawn->UpdateMeshVisibility(false);
		}

		PlayWeaponSoundCue(WeaponConfig.ZoomInfo.ZoomInSound);
	}
	else
	{
		// Show
		//if (MyPawn->IsFirstPersonView())
		if (GetViewerIsFirstPerson())
		{
			UpdateMeshVisibilitys(true);
			MyPawn->UpdateMeshVisibility(true);
		}

		PlayWeaponSoundCue(WeaponConfig.ZoomInfo.ZoomOutSound);
	}
}