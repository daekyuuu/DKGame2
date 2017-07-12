// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Weapons/PBWeaponTypes.h"
#include "Weapons/PBWeaponData.h"
#include "GameFramework/Actor.h"
#include "PBWeapon.generated.h"

class UPBWeapAction;
class UPBWeapAttachment;
class APBBulletBase;
class UPBWeapFanTrace;
class APBInst_MeleeBullet;
struct FPBWeaponTableData;

class UMeshComponent;
class UAudioComponent;
class UTexture;

UCLASS(Abstract, Blueprintable)
class APBWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	friend class UPBWeapAction;
	friend class UPBWeapActionFiringOnce;
	friend class UPBWeapActionFiringBurst;
	friend class UPBWeapActionMeleeAttack;
	friend class UPBWeapActionQuickThrow;

	friend class UPBWeapAttachment;
	friend class UPBWeapAttach_Dual;

	friend class UPBWeapFanTrace;

	/** perform initial setup */
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void UpdateMeshVisibilitys(bool bForce=false);
	virtual void ForceMeshVisibilitys(bool bVisible, bool bFirstPerson = false);
	//////////////////////////////////////////////////////////////////////////
	// Ammo
	APBBulletBase* SpawnBullet(EWeaponPoint ePoint, const FTransform& Transform);
	virtual void CreateBullets();
	APBInst_MeleeBullet* GetMeleeBullet() const;

	/** [server] add ammo */
	void GiveAmmo(int AddAmount);

	bool HasAmmoInClip() const;
	bool HasAmmo() const;

	/** Reset all of ammo to beginning state*/
	void FillAmmo();

	/** consume a bullet */
	void UseAmmo();

	// [server only] you must call it in OnFiring()
	void UpdateFirstFiringFlag();

	EWeaponType GetWeaponType() const;
	void SetWeaponType(EWeaponType type);

	void SetItemID(int32 ID);
	int32 GetItemID() { return WeaponConfig.ItemID;  }
	virtual bool ApplyTableData(const FPBWeaponTableData* Data);

	bool GetUseTableTata();
	FPBWeaponTableData* GetTableData();
	FPBWeaponTableData* RefTableData;

	UPBWeapAttachment* GetWeaponAttachment() { return WeapAttachment; }

	virtual float ApplyMovementSpeedModifier(float Speed);
	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(const APBWeapon* LastWeapon);

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(APBCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	virtual void SwitchToBestWeapon();

	//////////////////////////////////////////////////////////////////////////
	// Input
	// mouse Lbutton, gamepad right trigger button 
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadWrite, Category = "TriggerAction", meta = (DisplayName = "FireAction1"), NoClear)
	UPBWeapAction* FireTriggerAction1;

	// mouse Rbutton, gamepad right sholder button
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadWrite, Category = "TriggerAction", meta = (DisplayName = "FireAction2"), NoClear)
	UPBWeapAction* FireTriggerAction2;

	// weapon special ability
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadWrite, Category = "TriggerAction", meta = (DisplayName = "FunctionAction"))
	UPBWeapAction* FunctionalTriggerAction;

	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadWrite, Category = "TriggerAction", meta = (DisplayName = "MeleeAttackAction"))
	UPBWeapAction* MeleeAttackTriggerAction;

	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadWrite, Category = "TriggerAction", meta = (DisplayName = "QuickThrowAction"))
	UPBWeapAction* QuickThrowTriggerAction;

	/** [local] start weapon fire */
	virtual void FuncTriggerBegin();
	virtual void QuickThrowTriggerBegin(EWeaponType Type);
	virtual void QuickThrowTriggerEnd();
	virtual void MeleeAttackTriggerBegin();
	virtual void MeleeAttackTriggerEnd();	

	/** [local + server] start weapon fire */
	virtual void FireTriggerBegin(EFireTrigger eFireTrigger);

	/** [local + server] stop weapon fire */
	virtual void FireTriggerEnd(EFireTrigger eFireTrigger);
	//////////////////////////////////////////////////////////////////////////
	// Melee attack (개머리판, 찌르기 등)
	virtual void OnMeleeAttackStarted();
	virtual void OnMeleeAttackFinished();
	virtual void OnMeleeAttack();
	virtual void OnReMeleeAttack();
	virtual void OnPostMeleeAttackEnd();
	UFUNCTION(reliable, server, WithValidation)
	void ServerMeleeAttack();

	// melee hit, damage, effect
	// This is called by animation blueprint to match animation timing!
	UFUNCTION(BlueprintCallable, Category = "AttackHit")
	virtual void MeleeAttackHitTrace(EAttackDirType AttackDirType);
	virtual void OnMeleeAttackHit(const FHitResult& HitResult, const FVector& Origin, const FVector& ShootDir);
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyMeleeHit(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal ShootDir);
	
	float SinceFiring();
	virtual bool IsInFireInterval() const;

	// OnRep_FireReady 에서 Sync
	virtual void FireReady(EFireTrigger eFireTrigger);

	// if burstcount < 0, infinit
	virtual void StartFire(EFireTrigger eFireTrigger, int32 BurstCount=-1);
	virtual void EndFire(EFireTrigger eFireTrigger);

	/** [all] start weapon reload */
	virtual void StartReload();
	virtual void OnStartReload();

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** trigger reload from server */
	UFUNCTION(reliable, client)
	void ClientStartReload();
	
	//////////////////////////////////////////////////////////////////////////
	// Control

	virtual bool CanEquip() const;

	/** check if weapon can fire */
	virtual bool CanFire() const;
	virtual bool CanMeleeAttack() const;

	/** check if weapon can be reloaded */
	bool CanReload() const;

	virtual bool CanZooming() const;

	//////////////////////////////////////////////////////////////////////////
	// Recoil

private:
	void NotifyWeaponBurstRecoil(int BurstCounter);

	//////////////////////////////////////////////////////////////////////////
	// Deviation

public:
	float GetShootingDeviation();

	float GetCurrentDeviation();

private:
	float CurrentDeviationMultiplier;

	float CurrentFiringDeviation;

	void AddDeviationOnFire();

	void UpdateDeviation(float Delta);

	//////////////////////////////////////////////////////////////////////////
	// Expansion

public:
	float GetExpansionSize();

private:
	float CurrentExpansionMultiplier;

	float CurrentExpansionSize;

	void AddBurstEffectOnFire();

	void UpdateBurstEffect(float Delta);

	//////////////////////////////////////////////////////////////////////////
	// Reading data

public:
	int GetBurstCounter() const;
	void SetFirePoint(EWeaponPoint ePoint);
	EWeaponPoint GetFirePoint();
	void SetFireMode(EFireMode eMode);
	EFireMode GetFireMode();

	/** get current weapon state */
	EWeaponState GetCurrentState() const;

	// Set the ammo amount forcibly. You need to know what you are doing when you call this.
	void SetAmmoForcibly(int32 InCurrentAmmo, int32 InCurrentAmmoInClip);

	/** get current ammo amount (total) */
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	int32 GetMaxAmmo() const;

	/** get piercing power */
	float GetPiercingPower() const;

	FZoomInfo GetZoomInfo() const;

	virtual void SetZooming(bool bZoom);
	bool IsZooming() const;	

	bool IsReloading() const;
	bool IsEquiping() const;
	virtual bool IsFiring() const;
	bool IsMeleeAttacking() const;

	/** get weapon mesh (needs pawn owner to determine variant) */
	USkeletalMeshComponent* GetWeaponMesh(EWeaponPoint ePoint) const;
	USkeletalMeshComponent* GetSpecificWeapMesh(EWeaponPoint ePoint, bool bFirstPerson) const;
	void Get1PMeshes(TArray<UMeshComponent*>& Meshes);

	/** icon displayed on the HUD when weapon is equipped as primary */
	UPROPERTY(EditDefaultsOnly, Category=HUD)
	UTexture* DefaultIcon;

	/** icon displayed in death messages log when killed with this weapon */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	UTexture* KillIcon;
	
	/** check if weapon has infinite ammo (include owner's cheats) */
	bool HasInfiniteAmmo() const;
	void SetInfiniteAmmo(bool bSet);

	/** check if weapon has infinite clip (include owner's cheats) */
	bool HasInfiniteClip() const;
	void SetInfiniteClip(bool bSet);

	/** set the weapon's owning pawn */
	void SetOwningPawn(APBCharacter* APBCharacter);

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class APBCharacter* GetOwnerPawn() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Camera")
	bool GetViewerIsFirstPerson() const;

	/** gets last time when this weapon was switched to */
	float GetEquipStartedTime() const;

	/** gets the duration of equipping weapon*/
	float GetEquipDuration() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	EWeaponStance GetWeaponStance() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	const FWeaponBasePose& GetWeaponPose();

	float GetRecoilReturnRate() const;
	float GetCamRecoilRatio() const;
	float GetCamSyncRate() const;

	bool GetCrossHairPositionFixed() const;
	TSubclassOf<UPBCrossHairImageWidget> GetCrossHairImageWidgetClass() const;
	bool GetZoomCrossHairPositionFixed() const;
	TSubclassOf<UPBCrossHairImageWidget> GetZoomCrossHairImageWidgetClass() const;

	float GetZoomEyeSpeedMultiplier() const;
	float GetFireEyeSpeedMultiplier() const;

protected:

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category="Weapon Data", meta = (DisplayName="Settings"), NoClear)
	UPBWeapAttachment* WeapAttachment;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	EWeaponType WeaponType;	

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_MyPawn)
	class APBCharacter* MyPawn;

	UPROPERTY(EditDefaultsOnly, Category = Config, meta = (DisplayName="UseTableData (Need ItemID)"))
	bool UseTableDataToConfig;

	// 임시 관통 사용 플래그
	UPROPERTY(EditDefaultsOnly, Category = Config)
	bool bUsePierceWeapon;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponRecoilData WeaponRecoilConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponDeviationData WeaponDeviationConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponCrossHairData WeaponCrossHairConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponEyeSpeedData WeaponEyeSpeedConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stance")
	EWeaponStance WeaponStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stance")
	FWeaponBasePose WeaponPose;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stance")
	FWeaponBasePose WeaponPose_LobbyMale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stance")
	FWeaponBasePose WeaponPose_LobbyFemale;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	//** melee attack */
	TArray<bool> bWantsToMeleeAttack;

	/** is weapon fire active? */
	TArray<bool> bWantsToFire;

	/** is weapon fire active? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_FireReady)
	TArray<bool> bWantsToFireReady;

	// Local only
	UPROPERTY(Transient)
	bool bPostMeleeAttack;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
	uint32 bPendingReload : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	// PostFiring 에서 초기화 한다.
	bool bPendingFiring;

	// PostFiringEnd 에서 초기화 한다.
	bool bPostFiring;

	/** weapon is refiring */
	uint32 bRefiring : 1;

	/** current weapon state */
	EWeaponState PrevState;

	/** current weapon state */
	EWeaponState CurrentState;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** time of last successful weapon fire */
	float LastMeleeAttackTime;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** current total ammo */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCounter)
	FFireSync FiringSync;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MeleeAttack)
	FFireSync MeleeAttackSync;

	UPROPERTY(Transient)
	int32 DesiredBurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of Firing or melee attack timer */
	FTimerHandle TimerHandle_ReFiring;	
	FTimerHandle TimerHandle_PostFiringEnd;

	FTimerHandle TimerHandle_ReMeleeAttack;
	FTimerHandle TimerHandle_PostMeleeAttackEnd;

protected:
	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
	void ServerFireTriggerBegin(EFireTrigger eFireTrigger);

	UFUNCTION(reliable, server, WithValidation)
	void ServerFireTriggerEnd(EFireTrigger eFireTrigger);

	UFUNCTION(reliable, server, WithValidation)
	void ServerMeleeAttackTriggerBegin();

	UFUNCTION(reliable, server, WithValidation)
	void ServerMeleeAttackTriggerEnd();

	UFUNCTION(reliable, server, WithValidation)
	void ServerQuickThrowTriggerBegin(EWeaponType Type);

	UFUNCTION(reliable, server, WithValidation)
	void ServerQuickThrowTriggerEnd();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopReload();


	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	UFUNCTION()
	void OnRep_MyPawn();

	UFUNCTION()
	virtual void OnRep_BurstCounter();

	UFUNCTION()
	virtual void OnRep_MeleeAttack();
	

	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION()
	void OnRep_FireReady(TArray<bool> bPreWantsToFireReady);

	/** Called in network play to do the cosmetic fx for firing */
	virtual void PlayWeaponFireEffect(EWeaponPoint ePoint);

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopWeaponFireEffects();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage
protected:
	/** [local] weapon specific fire implementation */
	virtual void FireWeapon(EWeaponPoint ePoint) PURE_VIRTUAL(APBWeapon::FireWeapon,);

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	virtual void ServerFiring(EWeaponPoint ePoint);

	/** [local + server] handle weapon fire */
	virtual void PendingFiring(float PendingFire);
	virtual void PreFiring(EWeaponPoint ePoint);
	virtual void Firing(EWeaponPoint ePoint);
	virtual void PostFiring(EWeaponPoint ePoint);
	virtual void PostFiringEnd();
	virtual void SetReFiring(bool bReFire);

	virtual void OnFiring();

	/** [local + server] firing started */
	virtual void OnFireStarted(int32 BurstCount);

	/** [local + server] firing finished */
	virtual void OnFireFinished();

	virtual void SetWeaponFireState(EFireTrigger eFireTrigger, bool bFireBegin);
	virtual void SetWeaponFireReadyState(EFireTrigger eFireTrigger, bool bReady);
	virtual void SetWeaponAttackState(EFireTrigger eFireTrigger, bool bFireBegin);
	/** update weapon state */
	virtual void SetWeaponState(EWeaponState NewState);

	/** determine current weapon state */
	virtual void DetermineWeaponState(EFireTrigger eFireTrigger);
	virtual void DetermineWeaponStates();

	virtual void WakeFireTriggers();

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers

	void PlayFireForceFeedback(EWeaponPoint ePoint);
	void PlayFireCameraShake(EWeaponPoint ePoint);
	void PlayEquipCameraShake(EWeaponPoint ePoint= EWeaponPoint::Point_R);
	void StopEquipCameraShake(EWeaponPoint ePoint= EWeaponPoint::Point_R);
	void PlayReloadCameraShake(EWeaponPoint ePoint= EWeaponPoint::Point_R);
	void StopReloadCameraShake(EWeaponPoint ePoint= EWeaponPoint::Point_R);

	/** play weapon sounds */
	void PlayWeaponSounds(EWeaponSoundType eSound);
	UAudioComponent* PlayWeaponSound(EWeaponPoint ePoint, EWeaponSoundType eSound);

	UAudioComponent* PlayWeaponSoundCue(USoundCue* Sound);

public:
	/** play weapon animations */
	float PlayWeaponAnimation(EWeaponAnimType eAnim);
	/** stop playing weapon animations */
	void StopWeaponAnimation(EWeaponAnimType eAnim, bool GotoEndSection = false);

protected:
	/** check weapon animation length */
	float GetWeaponAnimationLength(EWeaponAnimType eAnim);
	float PlaySpecificWeaponAnim(USkeletalMeshComponent* UseMesh, const FWeaponAnim& Animation);
	void StopSpecificWeaponAnim(USkeletalMeshComponent* UseMesh, const FWeaponAnim& Animation, bool GotoEndSection=false);

	/** play anim montage */
	virtual float PlayAnimMontage(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	/** stop playing montage */
	virtual void StopAnimMontage(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage);

	virtual void MontageJumpToSection(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage, FName StartSectionName);

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the aim of the camera */
	FVector GetCameraAim() const;

	/** get the originating location for camera damage */
	virtual FVector GetStartFireLoc(const FVector& AimDir) const;
	EWeaponAnimType GetFireAnimType();
	
public:	

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;	

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
	TArray<FHitResult> WeaponTraces(const FVector& StartTrace, const FVector& EndTrace) const;

	// check crosshair targetting
	void AimingTrace(FHitResult& HitResult);	

	/** spawn effects for impact */
	void SpawnImpactEffect(const FHitResult& Impact, TSubclassOf<class APBImpactEffect> EffectTamplete);

	const FVector GetPaniniLocation(const FVector& InLocation, class UMaterialInterface* PaniniParamsMat);

	/* ------------------------------------------------------------------------------- */
	// Related with killmark
	/* ------------------------------------------------------------------------------- */

	// To implement "MassKill"
	// It will called by "Shotgun" or explosive weapon
	void ResetOneShotKillsNumber();

	/* ------------------------------------------------------------------------------- */
	// Static util functions
	/* ------------------------------------------------------------------------------- */
	UFUNCTION(BlueprintCallable, Category = "Weapoin")
	static bool IsGun(EWeaponType type);

	UFUNCTION(BlueprintCallable, Category = "Weapoin")
	static bool IsExplosive(EWeaponType type);

};

