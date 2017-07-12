// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPlayerCameraManager.h"
#include "Weapons/PBWeapon.h"

APBPlayerCameraManager::APBPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAlwaysApplyModifiers = true;
	bUseClientSideCameraUpdates = false;
	PrimaryActorTick.bTickEvenWhenPaused = true;

	CustomCameraStyle = EPBCameraStyle::None;
}

EPBCameraStyle APBPlayerCameraManager::GetCameraStyle() const
{
	return CustomCameraStyle;
}


void APBPlayerCameraManager::UpdateFOV(float DeltaTime, APBCharacter *FollowingCharacter, UCameraComponent *Cam)
{
	check(FollowingCharacter);
	check(Cam);

	// Update WeaponZoomAdjustedFOV by zoom and apply that to the final camera POV
	const APBWeapon *Weapon = FollowingCharacter->GetCurrentWeapon();
	if (Weapon)
	{
		const float TargetFOV = (Weapon && Weapon->GetZoomInfo().FOV > 0.f && FollowingCharacter->IsZooming()) ? Weapon->GetZoomInfo().FOV : DefaultFOV;
		const float ZoomInterpSpeed = UPBGameplayStatics::InterpDelayToInterpSpeed(Weapon ? Weapon->GetZoomInfo().InterpDelay : 0.f);
		WeaponZoomAdjustedFOV = FMath::FInterpTo(WeaponZoomAdjustedFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);
		float NewFOV = WeaponZoomAdjustedFOV;

		Cam->SetFieldOfView(NewFOV);
	}
}

void APBPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	APBCharacter *ViewPlayerCharacter = Cast<APBCharacter>(OutVT.Target);
	if (nullptr == ViewPlayerCharacter)
	{
		//UE_LOG(LogStats, Display, TEXT("ViewTargetInternal Target:%s / CamStyle(%s), PBCamType(%d)"), *OutVT.Target->GetName(), *CameraStyle.ToString(), int32(CustomCameraStyle));
		
		return Super::UpdateViewTargetInternal(OutVT, DeltaTime);
	}
	
	APBCharacter *FollowingCharacter = Cast<APBCharacter>(TargetToFollow.Get());
	if (nullptr == FollowingCharacter)
	{
		FollowingCharacter = ViewPlayerCharacter;
	}

	//UE_LOG(LogStats, Display, TEXT("Target:%s / CamStyle(%s), PBCamType(%d)"), *OutVT.Target->GetName(), *CameraStyle.ToString(), int32(CustomCameraStyle));
	
	const float InteropSpeed = 5.0f;

	if (CustomCameraStyle == EPBCameraStyle::RagDoll)
	{
		UCameraComponent *Cam = FollowingCharacter->CameraComp;
		check(Cam);

		FRotator NewRotation = FollowingCharacter->GetRagdollViewpointRotation();
		FVector NewLocation = FollowingCharacter->GetRagdollViewpointPosition();

		Cam->SetWorldRotation(NewRotation);
		Cam->SetWorldLocation(NewLocation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}
	else if (CustomCameraStyle == EPBCameraStyle::DeathCam)
	{
		//UCameraComponent *Cam = ViewPlayerCharacter->CameraComp;
		UCameraComponent *Cam = FollowingCharacter->CameraComp;
		check(Cam);

		//FRotator NewRotation = FollowingCharacter->GetControlRotation();
		//FRotator NewRotation = FollowingCharacter->GetFirstPersonViewTransform().Rotator();
		//FVector NewLocation = DeathCamPosition;

		FTransform NewTransform = FollowingCharacter->GetFirstPersonViewTransform();
		FRotator NewRotation = NewTransform.Rotator();
		FVector NewLocation = DeathCamPosition;

		Cam->SetWorldRotation(NewRotation);
		Cam->SetWorldLocation(NewLocation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}
	else if (CustomCameraStyle == EPBCameraStyle::FPS)
	{
		UCameraComponent *Cam = FollowingCharacter->CameraComp;
		check(Cam);

		UpdateFOV(DeltaTime, FollowingCharacter, Cam);

		FTransform NewTransform = FollowingCharacter->GetFirstPersonViewTransform();
		FRotator NewRotation = NewTransform.Rotator();
		FVector NewLocation = NewTransform.GetLocation();

		FTransform CamTransform = Cam->GetComponentTransform();
		NewRotation = FMath::RInterpTo(CamTransform.Rotator(), NewRotation, DeltaTime, InteropSpeed);

		Cam->SetWorldLocationAndRotation(NewLocation, NewRotation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}
	else if (CustomCameraStyle == EPBCameraStyle::TPS)
	{
		UCameraComponent *Cam = FollowingCharacter->ThirdPersonCameraComp;
		check(Cam);

		//UpdateFOV(DeltaTime, FollowingCharacter, Cam);

		FTransform NewTransform = FollowingCharacter->GetThirdPersonViewTransform();
		FRotator NewRotation = NewTransform.Rotator();
		FVector NewLocation = NewTransform.GetLocation();
		
		FTransform CamTransform = Cam->GetComponentTransform();
		NewRotation = FMath::RInterpTo(CamTransform.Rotator(), NewRotation, DeltaTime, InteropSpeed);
		NewLocation = FMath::VInterpTo(CamTransform.GetLocation(), NewLocation, DeltaTime, InteropSpeed);

		Cam->SetWorldLocationAndRotation(NewLocation, NewRotation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}
	else if (CustomCameraStyle == EPBCameraStyle::DebugFreeCam)
	{
		UCameraComponent *Cam = ViewPlayerCharacter->DebugFreeCamComp;
		check(Cam);

		auto CamTransform = FollowingCharacter->DebugFreeCamComp->GetComponentTransform();

		FQuat NewRotation = CamTransform.GetRotation();
		FVector NewLocation = CamTransform.GetLocation();

		Cam->SetWorldLocationAndRotation(NewLocation, NewRotation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}
	else if (CustomCameraStyle == EPBCameraStyle::DebugSphericalCam)
	{
		UCameraComponent *Cam = ViewPlayerCharacter->DebugSphericalCamComp;
		check(Cam);

		auto ArmTransform = FollowingCharacter->DebugSphericalCamSpringArm->GetSocketTransform(USpringArmComponent::SocketName);

		FQuat NewRotation = ArmTransform.GetRotation();
		FVector NewLocation = ArmTransform.GetLocation();

		Cam->SetWorldLocationAndRotation(NewLocation, NewRotation);
		Cam->GetCameraView(DeltaTime, OutVT.POV);

		return;
	}

	Super::UpdateViewTargetInternal(OutVT, DeltaTime);
}

void APBPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	if (CustomCameraStyle == EPBCameraStyle::DeathCam)
	{
		if (bInterpCameraToKiller
			&& !OutDeltaRot.IsNearlyZero())
		{
			// Player wants to look around, do not interpolate camera to killer
			bInterpCameraToKiller = false;
		}

		if (bPendingSetDeathCamInitialRotation)
		{
			OutViewRotation = DeathCamInitialRotation;
			OutDeltaRot = FRotator::ZeroRotator;

			bPendingSetDeathCamInitialRotation = false;
			return;
		}

		if (bInterpCameraToKiller && DeathCamKiller.IsValid())
		{
			FVector KillerLocation = DeathCamKiller->GetActorLocation();
			FVector TargetDirection = (KillerLocation - DeathCamPosition).GetSafeNormal();

			if (TargetDirection.Size() > 0.f
				&& !TargetDirection.Equals(FVector(0, 0, 1), SMALL_NUMBER)
				&& !TargetDirection.Equals(FVector(0, 0, -1), SMALL_NUMBER))
			{
				FVector RightVector = FVector::CrossProduct(TargetDirection, FVector(0, 0, -1)).GetSafeNormal();
				FRotator TargetRotation = FRotationMatrix::MakeFromXY(TargetDirection, RightVector).Rotator();
				OutViewRotation = FQuat::Slerp(OutViewRotation.Quaternion(), TargetRotation.Quaternion(), DeltaTime * 3).Rotator();
			}

			OutDeltaRot = FRotator::ZeroRotator;
			return;
		}
	}

	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}


void APBPlayerCameraManager::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Super::GetActorEyesViewPoint(Location, Rotation);
}

void APBPlayerCameraManager::EnterNullMode()
{
	CustomCameraStyle = EPBCameraStyle::None;
	
	TargetToFollow = nullptr;
}

void APBPlayerCameraManager::EnterFPSMode(APawn* InTargetToFollow)
{
	CustomCameraStyle = EPBCameraStyle::FPS;
	WeaponZoomAdjustedFOV = DefaultFOV;

	TargetToFollow = InTargetToFollow;
}

void APBPlayerCameraManager::EnterRagdollMode(APawn* InTargetToFollow)
{
	CustomCameraStyle = EPBCameraStyle::RagDoll;

	TargetToFollow = InTargetToFollow;
}

void APBPlayerCameraManager::EnterDeathCamMode(APawn *InTargetToFollow, AActor* Killer)
{
	CustomCameraStyle = EPBCameraStyle::DeathCam;

	TargetToFollow = InTargetToFollow;

	SetupDeathCamSpot();
	SetupDeathCamChase(Killer);
}

void APBPlayerCameraManager::SetupDeathCamSpot()
{
	APBCharacter* PBCharacter = TargetToFollow.IsValid() ? Cast<APBCharacter>(TargetToFollow.Get()) : Cast<APBCharacter>(GetViewTarget());
	if (nullptr == PBCharacter)
		return;

	const FVector PawnLocation = PBCharacter->GetFirstPersonViewTransform().GetLocation();
	FRotator ViewDir = PBCharacter->GetFirstPersonViewTransform().Rotator();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(TEXT("DeathCamera"), true, this);

	FHitResult HitResult;
	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		const bool bBlocked = GetWorld() ? GetWorld()->LineTraceSingleByChannel(HitResult, PawnLocation, TestLocation, ECC_Camera, TraceParams) : false;
		if (!bBlocked)
		{
			DeathCamPosition = TestLocation;
			DeathCamInitialRotation = CameraDir;
			bPendingSetDeathCamInitialRotation = true;
			return;
		}
	}

	DeathCamPosition = PBCharacter->GetActorLocation() + 3 * FVector(0, 0, PBCharacter->GetDefaultHalfHeight());
	DeathCamInitialRotation = FRotator(-90.f, 0.0f, 0.0f);
	bPendingSetDeathCamInitialRotation = true;
}

void APBPlayerCameraManager::SetupDeathCamChase(AActor* Killer)
{
	DeathCamKiller = Killer;
	bInterpCameraToKiller = true;
}

void APBPlayerCameraManager::EnterTPSMode(APawn* InTargetToFollow)
{
	CustomCameraStyle = EPBCameraStyle::TPS;

	TargetToFollow = InTargetToFollow;
}

void APBPlayerCameraManager::EnterDebugFreeCam(APawn *InTargetToFollow)
{
	CustomCameraStyle = EPBCameraStyle::DebugFreeCam;

	TargetToFollow = InTargetToFollow;
}

void APBPlayerCameraManager::EnterDebugSphericalCam(APawn *InTargetToFollow)
{
	CustomCameraStyle = EPBCameraStyle::DebugSphericalCam;
	
	TargetToFollow = InTargetToFollow;
}

bool APBPlayerCameraManager::IsViewing(APawn *PawnForComparing) const
{
	if (nullptr == PawnForComparing)
		return false;

	return TargetToFollow.Get() == PawnForComparing;
}

APawn *APBPlayerCameraManager::GetTargetToFollow() const
{
	if (TargetToFollow.IsValid())
		return TargetToFollow.Get();

	return nullptr;
}