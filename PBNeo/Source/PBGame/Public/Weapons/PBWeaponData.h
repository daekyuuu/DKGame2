#pragma once

#include "PBItemTable_Weap.h"
#include "Engine/EngineTypes.h"
#include "PBWeaponData.generated.h"

class UPBCrossHairImageWidget;
class USoundCue;
class UAnimMontage;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FWeaponEyeSpeedData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = EyeSpeed)
	float ZoomEyeSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = EyeSpeed)
	float FireEyeSpeedMultiplier;

	/** defaults */
	FWeaponEyeSpeedData()
	{
		ZoomEyeSpeedMultiplier = 1.f;
		FireEyeSpeedMultiplier = 1.f;
	}
};

USTRUCT(BlueprintType)
struct FWeaponCrossHairData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionBeginatShot (Table)"))
	int ExpansionBeginAtShot;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "BaseExpansion (Table)"))
	float BaseExpansion;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "MaxExpansion (Table)"))
	float MaxExpansion;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ZoomExpansionModifier (Table)"))
	float ZoomExpansionModifier;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionAddRateFire (Table)"))
	float ExpansionAddRate_Fire;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionReturnDelayFiring (Table)"))
	float ExpansionReturnDelay_Firing;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionReturnDelayNormal (Table)"))
	float ExpansionReturnDelay_Normal;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionRateWalk (Table)"))
	float ExpansionRateWalk;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionRateSprint (Table)"))
	float ExpansionRateSprint;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionRateCrouch (Table)"))
	float ExpansionRateCrouch;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair, meta = (DisplayName = "ExpansionRateJump (Table)"))
	float ExpansionRateJump;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair)
	bool CrossHairPositionFixed;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair)
	TSubclassOf<UPBCrossHairImageWidget> CrossHairImageWidget;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair)
	bool ZoomCrossHairPositionFixed;

	UPROPERTY(EditDefaultsOnly, Category = CrossHair)
	TSubclassOf<UPBCrossHairImageWidget> ZoomCrossHairImageWidget;

	/** defaults */
	FWeaponCrossHairData()
	{
		ExpansionBeginAtShot = 2;
		BaseExpansion = 0.f;
		MaxExpansion = 0.f;
		ZoomExpansionModifier = 0.0f;
		ExpansionAddRate_Fire = 0.0f;
		ExpansionReturnDelay_Firing = 3.f;
		ExpansionReturnDelay_Normal = 9.f;

		ExpansionRateWalk = 0.f;
		ExpansionRateSprint = 0.f;
		ExpansionRateCrouch = 0.f;
		ExpansionRateJump = 0.f;

		CrossHairPositionFixed = true;
		CrossHairImageWidget = TSubclassOf<UPBCrossHairImageWidget>();
		ZoomCrossHairPositionFixed = true;
		CrossHairImageWidget = TSubclassOf<UPBCrossHairImageWidget>();
	}

	bool ImportTableData(const FPBWeaponTableData* Data, bool IsDirty = false)
	{
		bool bIsDirty = IsDirty;

		if ((ExpansionBeginAtShot != Data->ExpansionBeginatShot) && !bIsDirty) { bIsDirty = true; }
		ExpansionBeginAtShot = Data->ExpansionBeginatShot;

		if ((BaseExpansion != Data->BaseExpansion) && !bIsDirty) { bIsDirty = true; }
		BaseExpansion = Data->BaseExpansion;

		if ((MaxExpansion != Data->MaxExpansion) && !bIsDirty) { bIsDirty = true; }
		MaxExpansion = Data->MaxExpansion;

		if ((ZoomExpansionModifier != Data->ZoomExpansionModifier) && !bIsDirty) { bIsDirty = true; }
		ZoomExpansionModifier = Data->ZoomExpansionModifier;

		if ((ExpansionAddRate_Fire != Data->ExpansionAddRateFire) && !bIsDirty) { bIsDirty = true; }
		ExpansionAddRate_Fire = Data->ExpansionAddRateFire;

		if ((ExpansionReturnDelay_Firing != Data->ExpansionReturnDelayFiring) && !bIsDirty) { bIsDirty = true; }
		ExpansionReturnDelay_Firing = Data->ExpansionReturnDelayFiring;

		if ((ExpansionReturnDelay_Normal != Data->ExpansionReturnDelayNormal) && !bIsDirty) { bIsDirty = true; }
		ExpansionReturnDelay_Normal = Data->ExpansionReturnDelayNormal;

		if ((ExpansionRateWalk != Data->ExpansionRateWalk) && !bIsDirty) { bIsDirty = true; }
		ExpansionRateWalk = Data->ExpansionRateWalk;

		if ((ExpansionRateSprint != Data->ExpansionRateSprint) && !bIsDirty) { bIsDirty = true; }
		ExpansionRateSprint = Data->ExpansionRateSprint;

		if ((ExpansionRateCrouch != Data->ExpansionRateCrouch) && !bIsDirty) { bIsDirty = true; }
		ExpansionRateCrouch = Data->ExpansionRateCrouch;

		if ((ExpansionRateJump != Data->ExpansionRateJump) && !bIsDirty) { bIsDirty = true; }
		ExpansionRateJump = Data->ExpansionRateJump;

		return bIsDirty;
	}
};

USTRUCT(BlueprintType)
struct FWeaponDeviationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationBeginatShot (Table)"))
	int DeviationBeginAtShot;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "BaseDeviation (Table)"))
	float BaseDeviation;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "MaxDeviation (Table)"))
	float MaxDeviation;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "ZoomDeviationModifier (Table)"))
	float ZoomDeviationModifier;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationAddRateFire (Table)"))
	float DeviationAddRate_Fire;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationReturnDelayFiring (Table)"))
	float DeviationReturnDelay_Firing;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationReturnDelayNormal (Table)"))
	float DeviationReturnDelay_Normal;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationRateWalk (Table)"))
	float DeviationRateWalk;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationRateSprint (Table)"))
	float DeviationRateSprint;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationRateCrouch (Table)"))
	float DeviationRateCrouch;

	UPROPERTY(EditDefaultsOnly, Category = Deviation, meta = (DisplayName = "DeviationRateJump (Table)"))
	float DeviationRateJump;

	UPROPERTY(EditDefaultsOnly, Category = WeaponData, meta = (DisplayName = "DeviationAddRateHit (Table)"))
	float DeviationAddRate_Hit;

	/** defaults */
	FWeaponDeviationData()
	{
		DeviationBeginAtShot = 2;
		BaseDeviation = 0.f;
		MaxDeviation = 0.f;
		ZoomDeviationModifier = 0.0f;
		DeviationAddRate_Fire = 0.0f;
		DeviationReturnDelay_Firing = 3.f;
		DeviationReturnDelay_Normal = 9.f;

		DeviationRateWalk = 0.f;
		DeviationRateSprint = 0.f;
		DeviationRateCrouch = 0.f;
		DeviationRateJump = 0.f;
		DeviationAddRate_Hit = 0.f;
	}

	bool ImportTableData(const FPBWeaponTableData* Data, bool IsDirty = false)
	{
		bool bIsDirty = IsDirty;

		if ((DeviationBeginAtShot != Data->DeviationBeginatShot) && !bIsDirty) { bIsDirty = true; }
		DeviationBeginAtShot = Data->DeviationBeginatShot;

		if ((BaseDeviation != Data->BaseDeviation) && !bIsDirty) { bIsDirty = true; }
		BaseDeviation = Data->BaseDeviation;

		if ((MaxDeviation != Data->MaxDeviation) && !bIsDirty) { bIsDirty = true; }
		MaxDeviation = Data->MaxDeviation;

		if ((ZoomDeviationModifier != Data->ZoomDeviationModifier) && !bIsDirty) { bIsDirty = true; }
		ZoomDeviationModifier = Data->ZoomDeviationModifier;

		if ((DeviationAddRate_Fire != Data->DeviationAddRateFire) && !bIsDirty) { bIsDirty = true; }
		DeviationAddRate_Fire = Data->DeviationAddRateFire;

		if ((DeviationReturnDelay_Firing != Data->DeviationReturnDelayFiring) && !bIsDirty) { bIsDirty = true; }
		DeviationReturnDelay_Firing = Data->DeviationReturnDelayFiring;

		if ((DeviationReturnDelay_Normal != Data->DeviationReturnDelayNormal) && !bIsDirty) { bIsDirty = true; }
		DeviationReturnDelay_Normal = Data->DeviationReturnDelayNormal;

		if ((DeviationRateWalk != Data->DeviationRateWalk) && !bIsDirty) { bIsDirty = true; }
		DeviationRateWalk = Data->DeviationRateWalk;

		if ((DeviationRateSprint != Data->DeviationRateSprint) && !bIsDirty) { bIsDirty = true; }
		DeviationRateSprint = Data->DeviationRateSprint;

		if ((DeviationRateCrouch != Data->DeviationRateCrouch) && !bIsDirty) { bIsDirty = true; }
		DeviationRateCrouch = Data->DeviationRateCrouch;

		if ((DeviationRateJump != Data->DeviationRateJump) && !bIsDirty) { bIsDirty = true; }
		DeviationRateJump = Data->DeviationRateJump;

		if ((DeviationAddRate_Hit != Data->DeviationAddRateHit) && !bIsDirty) { bIsDirty = true; }
		DeviationAddRate_Hit = Data->DeviationAddRateHit;

		return bIsDirty;
	}
};

USTRUCT(BlueprintType)
struct FWeaponRecoilData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilHorzBeginatShot (Table)"))
	int RecoilHorzBeginAtShot;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilHorzAngle (Table)"))
	float RecoilHorzAngle;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilHorzMaxAngle (Table)"))
	float RecoilHorzMaxAngle;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilVertMinAddition (Table)"))
	float RecoilVertMinAddition;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilVertAngle (Table)"))
	float RecoilVertAngle;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilVertMaxAngle (Table)"))
	float RecoilVertMaxAngle;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilReturnDelayFiring (Table)"))
	float RecoilReturnDelay_Firing;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "RecoilReturnDelayNormal (Table)"))
	float RecoilReturnDelay_Normal;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "CamRecoilRatio (Table)"))
	float CamRecoilRatio;

	UPROPERTY(EditDefaultsOnly, Category = Recoil, meta = (DisplayName = "CamSyncDelay (Table)"))
	float CamSyncDelay;

	/** defaults */
	FWeaponRecoilData()
	{
		RecoilHorzBeginAtShot = 5;
		RecoilHorzAngle = 0.f;
		RecoilHorzMaxAngle = 0.f;
		RecoilVertMinAddition = 0.f;
		RecoilVertAngle = 0.f;
		RecoilVertMaxAngle = 0.f;
		RecoilReturnDelay_Firing = 1.f;
		RecoilReturnDelay_Normal = 3.f;
		CamRecoilRatio = 2.f;
		CamSyncDelay = 1.f;
	}

	bool ImportTableData(const FPBWeaponTableData* Data, bool IsDirty = false)
	{
		bool bIsDirty = IsDirty;

		if ((RecoilHorzBeginAtShot != Data->RecoilHorzBeginatShot) && !bIsDirty) { bIsDirty = true; }
		RecoilHorzBeginAtShot = Data->RecoilHorzBeginatShot;

		if ((RecoilHorzAngle != Data->RecoilHorzAngle) && !bIsDirty) { bIsDirty = true; }
		RecoilHorzAngle = Data->RecoilHorzAngle;

		if ((RecoilHorzMaxAngle != Data->RecoilHorzMaxAngle) && !bIsDirty) { bIsDirty = true; }
		RecoilHorzMaxAngle = Data->RecoilHorzMaxAngle;

		if ((RecoilVertMinAddition != Data->RecoilVertMinAddition) && !bIsDirty) { bIsDirty = true; }
		RecoilVertMinAddition = Data->RecoilVertMinAddition;

		if ((RecoilVertAngle != Data->RecoilVertAngle) && !bIsDirty) { bIsDirty = true; }
		RecoilVertAngle = Data->RecoilVertAngle;

		if ((RecoilVertMaxAngle != Data->RecoilVertMaxAngle) && !bIsDirty) { bIsDirty = true; }
		RecoilVertMaxAngle = Data->RecoilVertMaxAngle;

		if ((RecoilReturnDelay_Firing != Data->RecoilReturnDelayFiring) && !bIsDirty) { bIsDirty = true; }
		RecoilReturnDelay_Firing = Data->RecoilReturnDelayFiring;

		if ((RecoilReturnDelay_Normal != Data->RecoilReturnDelayNormal) && !bIsDirty) { bIsDirty = true; }
		RecoilReturnDelay_Normal = Data->RecoilReturnDelayNormal;

		if ((CamRecoilRatio != Data->CamRecoilRatio) && !bIsDirty) { bIsDirty = true; }
		CamRecoilRatio = Data->CamRecoilRatio;

		if ((CamSyncDelay != Data->CamSyncDelay) && !bIsDirty) { bIsDirty = true; }
		CamSyncDelay = Data->CamSyncDelay;

		return bIsDirty;
	}
};

USTRUCT()
struct FZoomInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (DisplayName = "ZoomFOV (Table)"))
	float FOV;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (DisplayName = "ZoomInterpDelay (Table)"))
	float InterpDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Zoom)
	USoundCue* ZoomInSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Zoom)
	USoundCue* ZoomOutSound;

	FZoomInfo()
	{
		FOV = 90.f;
		InterpDelay = 20.f;
	}
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Weap1P;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Weap3P;

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn3P;	

	bool HasData() { return !(Weap1P == nullptr && Weap3P == nullptr && Pawn1P == nullptr && Pawn3P == nullptr); }
	FWeaponAnim()
	{
		Weap1P = nullptr;
		Weap3P = nullptr;
		Pawn1P = nullptr;
		Pawn3P = nullptr;
	}
};
static FWeaponAnim WeaponAnimNone;

USTRUCT()
struct FFireSync
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Transient)
	int32 BurstCounter;
	
	UPROPERTY(Transient)
	EWeaponPoint FirePoint;

	UPROPERTY(Transient)
	EFireMode FireMode;

	FFireSync()
	{
		BurstCounter = 0;
		FirePoint = EWeaponPoint::Point_R;
		FireMode = EFireMode::Single;
	}
};

USTRUCT()
struct FWeaponBasePose
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* Anim1P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* Anim1P_Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* Anim3P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimSequence* Anim3P_Zoom;

	bool HasData() { return !(Anim1P == nullptr && Anim1P_Zoom == nullptr && Anim3P == nullptr && Anim3P_Zoom == nullptr); }

	FWeaponBasePose()
	{
		Anim1P = nullptr;
		Anim1P_Zoom = nullptr;
		Anim3P = nullptr;
		Anim3P_Zoom = nullptr;
	}
};
static FWeaponBasePose WeaponPoseNone;

USTRUCT()
struct FWeaponMesh
{
	GENERATED_USTRUCT_BODY()

	/** weapon mesh: 1st person view */
	UPROPERTY(EditAnywhere, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(EditAnywhere, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

	FWeaponMesh()
	{
		Mesh1P = nullptr;
		Mesh3P = nullptr;
	}
};

USTRUCT()
struct FWeaponScale
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FVector Scale1P;
	
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FVector Scale3P;

	FWeaponScale()
	{
		Scale1P = FVector(1.f);
		Scale3P = FVector(1.f);
	}
};

USTRUCT()
struct FWeaponFXData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* FX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FVector FXScale;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	uint32 bFXLooped : 1;

	FWeaponFXData()
	{
		FX = nullptr;
		FXScale = FVector(1.f);
		bFXLooped = false;
	}
};

USTRUCT()
struct FWeaponMuzzleFX
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FWeaponFXData Data1P;
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FWeaponFXData Data3P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName FXPointName;

	FWeaponMuzzleFX()
	{
		FXPointName = FName(TEXT("FX_Muzzle"));
	}
};

USTRUCT()
struct FBulletFX
{
	GENERATED_USTRUCT_BODY()

	/** impact effects */
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Effects")
	TSubclassOf<class APBImpactEffect> ImpactTemplate;

	/** smoke trail */
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Effects")
	UParticleSystem* TrailFX_1P;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet Effects")
	UParticleSystem* TrailFX_3P;

	/** param name for beam target in smoke trail */
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Effects")
	FName TrailParamName;

	FBulletFX()
	{
		TrailFX_1P = nullptr;
		TrailFX_3P = nullptr;
		TrailParamName = FName(TEXT("ShockBeamEnd"));
	}
};

USTRUCT()
struct FWeaponSounds
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	uint32 bLoopedFireSound : 1;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	float FireSoundFadeOutTime;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireLoopSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* OutOfAmmoSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* MeleeAttackSound;

	FWeaponSounds()
	{
		bLoopedFireSound = false;
		FireSoundFadeOutTime = 0.3f;
	}
};

USTRUCT()
struct FWeaponAnims
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	uint32 bLoopedFireAnim : 1;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim FireAnim_Zoom;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim MeleeAttackAnim;

	bool HasData() { return (FireAnim.HasData() || FireAnim_Zoom.HasData() || ReloadAnim.HasData() || EquipAnim.HasData() || MeleeAttackAnim.HasData()); }

	FWeaponAnims()
	{
		bLoopedFireAnim = false;
	}
};

USTRUCT()
struct FLobbyWeaponAnims
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim WalkingStartAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim WalkingFinishAnim;

	bool HasData() { return (EquipAnim.HasData() || WalkingStartAnim.HasData() || WalkingFinishAnim.HasData()); }

};

USTRUCT()
struct FWeaponModeSetClass
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Inventory, meta = (DisplayName="WeaponModeSet"))
	TArray<TSubclassOf<class APBWeapon> > Classes;
};

// 모든 무기는 ModeSet 로 구성되어 있다
USTRUCT()
struct FWeaponModeSet
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<class APBWeapon*> Weapons; //weapon mode1, mode2 

	bool IsEmpty() { return (Weapons.Num() == 0); }

	FWeaponModeSet()
	{
		Weapons.Empty();
	}
};
static FWeaponModeSet EmptyWeapModeSet;

USTRUCT()
struct FWeaponSlotSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Transient)
	TArray<FWeaponModeSet> SlotWeapons;
	FWeaponSlotSet()
	{
		SlotWeapons.Empty();
	}
};

USTRUCT()
struct FWeaponMelee
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Config, meta = (DisplayName = "MeleeAttackBullet"))
	TSubclassOf<class APBInst_MeleeBullet> MeleeBulletClass;

	FWeaponMelee()
	{
		MeleeBulletClass = nullptr;
	}
};

// 관통샷 Hit 데이터를 묶어놓은 구조체
USTRUCT()
struct FPierceHitResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FHitResult Impact;

	UPROPERTY()
	FHitResult ImpactPierce;

	UPROPERTY()
	bool IsFirstHit;

	UPROPERTY()
	float ResultPiercingPower;

	FPierceHitResult()
	{
		IsFirstHit = false;
	}
};

USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector_NetQuantize100 Origin;

	UPROPERTY()
	FVector_NetQuantizeNormal ShootDir;

	//UPROPERTY()
	//int32 RepCounter;
};

USTRUCT()
struct FInstantPierceHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector_NetQuantize100 Origin;

	UPROPERTY()
	FVector_NetQuantizeNormal ShootDir;

	UPROPERTY(Transient)
	TArray<FPierceHitResult> PierceHitResult;
};


//////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	int32 ItemID;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	bool bInfiniteAmmo;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	bool bInfiniteClip;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "TotalAmmo (Table)"))
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "MeleeAttackInterval (Table)"))
	float MeleeAttackInterval;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "BulletFireInterval (Table)"))
	float FireInterval;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "MagazineCapacity (Table)"))
	int32 AmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "MovementSpeedModifierBase (Table)"))
	float MovementSpeedModifierBase;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "MovementSpeedModifierOnFire (Table)"))
	float MovementSpeedModifierOnFire;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "EquipDuration (Table)(if 0, use animation length)"))
	float EquipDuration;
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "ReloadDuration (Table)(if 0, use animation length)"))
	float ReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat, meta = (DisplayName = "Piercing Power (Table)"))
	float PiercingPower;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	FZoomInfo ZoomInfo;

	FWeaponData()
	{
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 100;
		AmmoPerClip = 20;
		FireInterval = 0.2f;
		MeleeAttackInterval = 1.f;
		EquipDuration = 0.f;
		ReloadDuration = 0.f;
		MovementSpeedModifierBase = 0.0f;
		MovementSpeedModifierOnFire = 0.0f;
		PiercingPower = 0.0f;
	}

	bool ImportTableData(const FPBWeaponTableData* Data, bool IsDirty = false)
	{
		bool bIsDirty = IsDirty;

		if ((MaxAmmo != Data->TotalAmmo) && !bIsDirty) { bIsDirty = true; }
		MaxAmmo = Data->TotalAmmo;

		if ((AmmoPerClip != Data->MagazineCapacity) && !bIsDirty) { bIsDirty = true; }
		AmmoPerClip = Data->MagazineCapacity;

		if ((FireInterval != Data->BulletFireInterval) && !bIsDirty) { bIsDirty = true; }
		FireInterval = Data->BulletFireInterval;

		if ((MeleeAttackInterval != Data->MeleeAttackInterval) && !bIsDirty) { bIsDirty = true; }
		MeleeAttackInterval = Data->MeleeAttackInterval;

		if ((MovementSpeedModifierBase != Data->MovementSpeedModifierBase) && !bIsDirty) { bIsDirty = true; }
		MovementSpeedModifierBase = Data->MovementSpeedModifierBase;

		if ((MovementSpeedModifierOnFire != Data->MovementSpeedModifierOnFire) && !bIsDirty) { bIsDirty = true; }
		MovementSpeedModifierOnFire = Data->MovementSpeedModifierOnFire;

		if ((ZoomInfo.FOV != Data->ZoomFOV) && !bIsDirty) { bIsDirty = true; }
		ZoomInfo.FOV = Data->ZoomFOV;

		if ((ZoomInfo.InterpDelay != Data->ZoomInterpDelay) && !bIsDirty) { bIsDirty = true; }
		ZoomInfo.InterpDelay = Data->ZoomInterpDelay;

		if ((EquipDuration != Data->EquipDuration) && !bIsDirty) { bIsDirty = true; }
		EquipDuration = Data->EquipDuration;

		if ((ReloadDuration != Data->ReloadDuration) && !bIsDirty) { bIsDirty = true; }
		ReloadDuration = Data->ReloadDuration;

		if ((PiercingPower != Data->PiercingPower) && !bIsDirty) { bIsDirty = true; }
		PiercingPower = Data->PiercingPower;

		return bIsDirty;
	}
};
//////////////////////////////////////////////////////////////////////////
// 새로운 구조체는 아래말고 위쪽에 추가해주세요.