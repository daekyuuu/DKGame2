// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBDamageType.generated.h"

// DamageType class that specifies an icon to display
UCLASS(const, Blueprintable, BlueprintType)
class UPBDamageType : public UDamageType
{
	GENERATED_UCLASS_BODY()

	// Deprecated: bScaleMomentumByMass
	// This parameter has unclear intrinsic meaning
	// and we will not use ACharacter::ApplyDamageMomentum(...)

	/* ------------------------------------------------------------------------------- */
	// GamePad Áøµ¿
	/* ------------------------------------------------------------------------------- */
	
	/** force feedback effect to play on a player hit by this damage type */
	UPROPERTY(EditDefaultsOnly, Category=Impact)
	UForceFeedbackEffect *HitForceFeedback;

	/** force feedback effect to play on a player killed by this damage type */
	UPROPERTY(EditDefaultsOnly, Category= Impact)
	UForceFeedbackEffect *KilledForceFeedback;

	// 
	UPROPERTY(EditDefaultsOnly, Category= Impact)
	TSubclassOf <UCameraShake> HitCameraShake;

	/* ------------------------------------------------------------------------------- */
	// DeathRagdoll
	/* ------------------------------------------------------------------------------- */

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float DeathImpulseOverNormalImpulse;

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float DeathJumpImpulseSize;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};

UCLASS(const, Blueprintable, BlueprintType)
class UPBFallOffDamageType : public UPBDamageType
{
	GENERATED_UCLASS_BODY()
};

UCLASS(const, Blueprintable, BlueprintType)
class UPBPointDamageType : public UPBDamageType
{
	GENERATED_UCLASS_BODY()

	/* ------------------------------------------------------------------------------- */
	// DeathRagdoll
	/* ------------------------------------------------------------------------------- */

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float ImpulseMultiplier_ShootDir;
	
	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float ImpulseMultiplier_HitDir;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};

UCLASS(const, Blueprintable, BlueprintType)
class UPBRadialDamageType : public UPBDamageType
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float ExtraDeathJumpImpulseFromDistance;
};



USTRUCT(BlueprintType)
struct FTakeDamageInfo
{
	GENERATED_USTRUCT_BODY()

	// DamageType has a id of causer item.
	UPROPERTY()
	int32 ItemId;

	UPROPERTY()
	class AActor* Victim;

	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	bool bIsHeadShot;

	UPROPERTY()
	bool bFallOff;

	FTakeDamageInfo()
		: ItemId(0), Victim(nullptr), ActualDamage(0.0f), bIsHeadShot(false)
	{
	}
};


/** replicated information on a hit we've taken */
USTRUCT(BlueprintType)
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** The amount of damage actually applied */
	UPROPERTY(BlueprintReadOnly, Category = TakeHitInfo)
	float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
	UClass* DamageTypeClass;

	/** Who hit us (attacker, killer's character pointer) */
	UPROPERTY(BlueprintReadOnly, Category = TakeHitInfo)
	TWeakObjectPtr<class APBCharacter> PawnInstigator;

	/** Who actually caused the damage (weapon, bullet, projectile ..)*/
	UPROPERTY(BlueprintReadOnly, Category = TakeHitInfo)
	TWeakObjectPtr<class AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
	int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY(BlueprintReadOnly, Category = TakeHitInfo)
	uint32 bKilled : 1;

private:
	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
	uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo()
		: ActualDamage(0)
		, DamageTypeClass(NULL)
		, PawnInstigator(NULL)
		, DamageCauser(NULL)
		, DamageEventClassID(0)
		, bKilled(false)
		, EnsureReplicationByte(0)
	{}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}

		DamageTypeClass = DamageEvent.DamageTypeClass;
	}

	bool PointDamageDataValid()
	{
		return (DamageEventClassID == FPointDamageEvent::ClassID);
	}
	FPointDamageEvent& GetPointDamageEvent()
	{
		if (PointDamageEvent.DamageTypeClass == NULL)
		{
			PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return PointDamageEvent;
	}

	bool RadialDamageDataValid()
	{
		return (DamageEventClassID == FRadialDamageEvent::ClassID);
	}
	FRadialDamageEvent& GetRadialDamageEvent()
	{
		if (RadialDamageEvent.DamageTypeClass == NULL)
		{
			RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return RadialDamageEvent;
	}

	FDamageEvent& GetDamageEvent()
	{
		if (PointDamageDataValid())
			return GetPointDamageEvent();

		if (RadialDamageDataValid())
			return GetRadialDamageEvent();

		if (GeneralDamageEvent.DamageTypeClass == NULL)
		{
			GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return GeneralDamageEvent;
	}

	void EnsureReplication()
	{
		EnsureReplicationByte++;
	}
};

