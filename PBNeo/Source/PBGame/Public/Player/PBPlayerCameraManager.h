// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBPlayerCameraManager.generated.h"

UENUM(BlueprintType)
enum class EPBCameraStyle : uint8
{
	None = 0,
	RagDoll,
	DeathCam,
	FPS,
	TPS,
	DebugFreeCam,
	DebugSphericalCam,
	Max,
};

UCLASS()
class APBPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

	void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;
	void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;

private:
	void UpdateFOV(float DeltaTime, APBCharacter *FollowingCharacter, UCameraComponent *Cam);

protected:

	EPBCameraStyle CustomCameraStyle;
public:
	EPBCameraStyle GetCameraStyle() const;

	/* ------------------------------------------------------------------------------- */
	// Null mode
	/* ------------------------------------------------------------------------------- */

public:

	void EnterNullMode();

	/* ------------------------------------------------------------------------------- */
	// FPS Mode
	/* ------------------------------------------------------------------------------- */

public:

	void EnterFPSMode(APawn* TargetToFollow);

protected:

	float WeaponZoomAdjustedFOV;

	/* ------------------------------------------------------------------------------- */
	// Rag-doll
	/* ------------------------------------------------------------------------------- */

public:

	void EnterRagdollMode(APawn* TargetToFollow);

	/* ------------------------------------------------------------------------------- */
	// DeathCam
	/* ------------------------------------------------------------------------------- */

public:
	
	void EnterDeathCamMode(APawn *InTargetToFollow, AActor* Killer);

protected:

	void SetupDeathCamSpot();
	FVector DeathCamPosition;
	FRotator DeathCamInitialRotation;
	bool bPendingSetDeathCamInitialRotation;

	void SetupDeathCamChase(AActor* Killer);
	UPROPERTY()
	TWeakObjectPtr<AActor> DeathCamKiller;
	bool bInterpCameraToKiller;

	/* ------------------------------------------------------------------------------- */
	// Follower
	/* ------------------------------------------------------------------------------- */

public:

	void EnterTPSMode(APawn* InTargetToFollow);

protected:

	UPROPERTY()
	TWeakObjectPtr<APawn> TargetToFollow;

public:
	APawn *GetTargetToFollow() const;

public:
	bool IsViewing(APawn *PawnForComparing) const;

	/* ------------------------------------------------------------------------------- */
	// Debug
	/* ------------------------------------------------------------------------------- */

public:

	void EnterDebugFreeCam(APawn *InTargetToFollow);

	void EnterDebugSphericalCam(APawn *InTargetToFollow);


	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */

};
