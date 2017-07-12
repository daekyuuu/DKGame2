// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBTypes.h"
#include "Weapons/PBWeaponTypes.h"
#include "Weapons/PBWeaponData.h"
#include "Weapons/PBDamageType.h"
#include "Player/PBCharacterTypes.h"
#include "Player/PBCharacterData.h"
#include "Player/PBPlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "PBCharacter.generated.h"

class APBWeapon;
class APBPlayerController;

UCLASS()
class UPBSceneComponent : public USceneComponent
{
	GENERATED_BODY()

	UPBSceneComponent();
};

UCLASS(Abstract)
class APBCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	virtual void UpdateMeshVisibility(bool bForce);
	virtual void UpdateWeaponVisibility(bool bForce);
	
	//for Lobby Character scene
	virtual void ForceMeshVisibilityWithWeapon(bool bVisible, bool bFirstPerson=false);

	void SetItemID(int32 ID) { CharacterData.ItemID = ID; }
	int32 GetItemID() { return CharacterData.ItemID; }
	virtual bool ApplyTableData(const struct FPBCharacterTableData* Data);
	struct FPBCharacterTableData* GetTableData();
	struct FPBCharacterTableData *RefTableData;
	FCharacterData& GetCharaterData() { return CharacterData; }

	bool GetUseTableTata();
	
	UPROPERTY(EditDefaultsOnly, Category = Config, meta = (DisplayName = "UseTableData (Need ItemID)"))
	bool UseTableDataToConfig;

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

	/** Update the character. (Sprinting, health etc). */
	virtual void Tick(float DeltaSeconds) override;

	/** cleanup inventory */
	virtual void Destroyed() override;

	/** update mesh for first person view */
	virtual void PawnClientRestart() override;

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) override;

	/** [client] perform PlayerState related setup */
	virtual void OnRep_PlayerState() override;

	/** get aim offsets */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	FRotator Get1PAimOffset() const;

	/** get aim offsets */
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	FRotator Get3PAimOffset() const;

	bool IsEnemyFor(AController* TestPC) const;

	bool IsBot();

	//////////////////////////////////////////////////////////////////////////
	// Inventory
	/** [server] spawns default inventory */
	void SpawnDefaultInventory(); //in Editor
	void SpawnUserInventory(AController* PlayerController); // With Lobby

	void AddWeaponBySlotClasses(EWeaponSlot eSlot, TArray<FWeaponModeSetClass>& SlotWeaponClasses);
	void AddWeaponByModeSetClass(EWeaponSlot eSlot, FWeaponModeSetClass& WeapModeSetClass, int32 SlotIdx=-1);
	APBWeapon* NewWeapon(TSubclassOf<APBWeapon> WeapClass);
	void AddWeapon(EWeaponSlot eSlot, FWeaponModeSet WeapModeSet, int32 SlotIdx = -1);
	void RemoveWeaponSlot(EWeaponSlot eSlot);
	void RemoveWeaponSlotIdx(EWeaponSlot eSlot, int32 SlotIdx);
	void DestroyWeapon(FWeaponModeSet& WeapModeSet);

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();	

	APBWeapon* FindWeaponByType(TSubclassOf<APBWeapon> WeaponClass);
	TArray<FWeaponModeSet>& GetSlotWeapons(EWeaponSlot eSlot);
	FWeaponModeSet& GetSlotIdxWeapons(EWeaponSlot eSlot, int32 SlotIdx);
	APBWeapon* GetWeapon(EWeaponSlot eSlot, int32 SlotIdx = 0, EWeaponMode eMode = EWeaponMode::Mode1);
	APBWeapon* GetGrenadeWeapon();
	APBWeapon* GetSpecialWeapon();


	bool GetWeaponSlotWithIdxMode(APBWeapon* EquippedWeapon, EWeaponSlot& outSlot, int32& outSlotIdx, EWeaponMode& outMode);
	EWeaponSlot GetWeaponSlot(APBWeapon* EquippedWeapon);
	int32 GetWeaponSlotIdx(APBWeapon* EquippedWeapon);
	EWeaponMode GetWeaponMode(APBWeapon* EquippedWeapon);

	EWeaponSlot GetCurrentWeaponSlot();
	int32		GetCurrentWeaponSlotIdx();	
	EWeaponMode GetCurrentWeaponMode();

	UFUNCTION(BlueprintNativeEvent, Category = Pawn)
	void NotifyEquipWeapon(APBWeapon* EquippedWeapon);

	void EquipWeaponItem(int32 ItemID, int32 ForceSlotIdx = -1);
	void EquipWeapon(EWeaponSlot eSlot, int32 SlotIdx = 0, EWeaponMode eMode = EWeaponMode::Mode1);
	bool SwitchWeapon(EWeaponSlot eSlot, int32 SlotIdx = 0, EWeaponMode eMode = EWeaponMode::Mode1);
	bool SwitchToBestWeapon();	

	/* ------------------------------------------------------------------------------- */
	// Character Pickup Action
	/* ------------------------------------------------------------------------------- */

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	class UPBActionPickup* PickupAction;

	void InitActionPickup();

public:
	class UPBActionPickup* GetActionPickup() const { return PickupAction; }
	virtual void PickedUpWeapon(TSubclassOf<APBWeapon> WeaponClass);
	virtual void PickedUpWeapon(TSubclassOf<APBWeapon> WeaponClass, int32 InCurrentAmmo, int32 InCurrentAmmoInClip);

protected:

	/** updates current weapon */
	void SetCurrentWeapon(class APBWeapon* NewWeapon, class APBWeapon* LastWeapon = nullptr);

	void EquipWeaponInternal(class APBWeapon* Weapon);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class APBWeapon* LastWeapon);

	// Quick throwing weapon or Quick knife
	UFUNCTION()
	void OnRep_CurrentQuickWeapon(class APBWeapon* LastQuickWeapon);

	/** equip weapon */
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class APBWeapon* NewWeapon);

public:

	void ToggleSwitchWeaponMode();
	bool SwitchWeaponMode(EWeaponMode eMode);
	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] starts weapon fire */
	void WakeFireTrigger();

	/** [local] starts weapon fire */
	void StartWeaponFire(EFireTrigger eFireTrigger = EFireTrigger::Fire1);

	/** [local] stops weapon fire */
	void StopWeaponFire(EFireTrigger eFireTrigger = EFireTrigger::Fire1);

	/** check if pawn can fire weapon */
	bool CanFire() const;

	/** check if pawn can reload weapon */
	bool CanReload() const;

	bool CanZooming() const;

	/** [server + local] change Zooming state */
	void SetZooming(bool bZoom);
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void BroadcastWidgetEventZooming(bool bZoom);
	
	/** update Zooming state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetZooming(bool bZoom);

	void InitVoiceSound();

	UFUNCTION(BlueprintCallable, Category = Sound)
	UAudioComponent* PlayVoice(EPBVoiceType VoiceType);

	bool StartQuickThrow(class APBWeap_Throw* ThrowWeapon);
	void StopQuickThrow(class APBWeap_Throw* ThrowWeapon);
	void OnQuickThrowEnd();
	
	//////////////////////////////////////////////////////////////////////////
	// Animations

	UFUNCTION(reliable, Server, WithValidation)
	void ServerPlayAnimation(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(reliable, Server, WithValidation)
	void ServerStopAnimation(class UAnimMontage* AnimMontage, bool GotoEndSection=false);

	/** play anim montage */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	/** stop playing montage */
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	/** stop playing all montages */
	void StopAllAnimMontages();

	virtual void MontageJumpToSection(class UAnimMontage* AnimMontage, FName StartSectionName);
	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	/** setup pawn specific input handlers */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//void OnShowPlayerDebugContexts();
private:
	bool bShiftDebugViewInput;
public:
	void OnPress_ShiftDebugViewInput();
	void OnRelease_ShiftDebugViewInput();

	void ChangeView(EPBCameraStyle CameraStyle);
	
	EPBCameraStyle GetMinCameraStyle() const;
	EPBCameraStyle GetMaxCameraStyle() const;

	void OnChangePreviousView_Debug();
	void OnChangeNextView_Debug();
	void OnChangeViewOnSpectator();
	void OnResetToFPView();
	void OnDebugViewZoom(float Val);

	void OnMoveForward(float Val);
	void ConditionalStopSprint(bool bForceStop = false);
	void OnMoveRight(float Val);
	void OnMoveUp(float Val);

	void OnStartWalking();
	void OnStopWalking();

	void OnStartSprinting();
	void OnStopSprinting();
	void OnSprintToggle();
	
	void OnStartFireTrigger1();
	void OnEndFireTrigger1();
	void OnStartFireTrigger2();
	void OnEndFireTrigger2();

	void OnWeapFuncTrigger();
	void OnStartMeleeAttackTrigger();
	void OnEndMeleeAttackTrigger();

	void OnStartQuickThrowTrigger(EWeaponType Type);
	void OnStopQuickThrowTrigger();

	void HandleStartQuickThrow();
	void HandleStartQuickGrenade();
	void HandleStartQuickSpecial();

	void OnPressPickup();
	void OnReleasePickup();


	/** player pressed Zooming action */
	void OnZoomIn();

	/** player released Zooming action */
	void OnZoomOut();

	/** player pressed next weapon action */
	void OnNextWeapon();

	/** player pressed prev weapon action */
	void OnPrevWeapon();

	void OnSwitchWeapon();

	/** Player want to switch the weapon which is fired when the player pressed the quick throw button. */
	void OnSwitchQuickWeapon();

	/** player pressed reload action */
	void OnReload();

	/** player pressed jump action */
	void OnStartJump();

	/** player released jump action */
	void OnStopJump();

	/** [local] player equipped the weapon */
	void OnEquipFinished();

	/** [local] Player ended the reload action */
	void OnReloadEnded();


	/* ------------------------------------------------------------------------------- */
	// Crouching
	/* ------------------------------------------------------------------------------- */

	void OnCrouch();
	void OnUnCrouch();
	void OnCrouchToggle();

	//  FirstPerson ViewPoint
	virtual void OnStartCrouch(float HeightAdjust, float ScaledHeightAdjust) override;
	virtual void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsCrouching() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	void PlayFootstep(EPBFootstepType StepType, bool bLeft);

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	void SetLobbyAnimClass();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	class TSubclassOf<UAnimInstance> LobbyAnimClass;

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return GetMesh(); }

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	class APBWeapon* GetCurrentWeapon() const;
	class APBWeapon* GetCurrentQuickWeapon();

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	EWeaponType GetCurrentWeaponType() const;

	/** get weapon attach point */
	FName GetWeaponAttachPointName(EWeaponPoint ePoint) const;

	/** get total number of inventory items */
	int32 GetInventoryCount() const;
	int32 GetSlotWeaponsCount(EWeaponSlot eSlot);

	/** get weapon taget modifier speed	*/
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	float GetZoomingSpeedModifier() const;

	/** get Zooming state */
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	bool IsZooming() const;

	bool IsWantsToZooming() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool GetViewerIsZooming() const;

	UFUNCTION()
	void OnRep_ZoomState();

	/** get firing state */
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	bool IsFiring() const;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category=Mesh)
	virtual bool IsFirstPersonView() const;
	UFUNCTION(BlueprintCallable, Category=Camera)
	bool IsViewingOwnself() const;
	UFUNCTION(BlueprintCallable, Category = Camera)
	bool GetViewerIsFirstPerson() const;
	UFUNCTION(BlueprintCallable, Category = Camera)
	EPBCameraStyle GetCameraStyle() const;
	void SetCameraStyle(EPBCameraStyle CameraStyle);
	UFUNCTION(BlueprintCallable, Category = Camera)
	bool IsCameraStyle(EPBCameraStyle CameraStyle) const;

	/** get max health */
	int32 GetMaxHealth() const;
	float GetHealth() const;
	
	/** Get health percentage (current/max [0,1]) */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercentage() const;

	/** returns percentage of health when low health effects should start */
	float GetLowHealthPercentage() const;

	/** check if pawn is still alive */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsSpectator() const;
	UFUNCTION(BlueprintCallable, Category = Pawn)
	void SetSpectator(bool bNewSpectator);
	/*
 	 * Get either first or third person mesh. 
	 *
 	 * @param	WantFirstPerson		If true returns the first peron mesh, else returns the third
	 */
	USkeletalMeshComponent* GetSpecificPawnMesh( bool WantFirstPerson ) const;

	/** Update the team color of all player meshes. */
	void UpdateTeamColorsAllMIDs();

	UFUNCTION(BlueprintCallable, Category = "CharacterInfo")
	EPBTeamType GetTeamType() const;
	bool IsSameTeam(EPBTeamType eOtherTeam);
	void TakeAimingTrace();
	FHitResult& GetAimingTraceResult();

	bool IsAimingTargeted() { return bIsAimingTargeted; }
	void SetAimingTargeted(bool bSet);

	APBCharacter *GetAmingTargetCache() const;

	virtual float ApplyDamagedSpeed(float Speed) const;
	float ApplyMovementSpeedModifier(float Speed) const;
	
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override;
	virtual void Falling() override;
	void OnFalling();
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
private:
	struct FFallDamage
	{
		float AccumulatedFallDeathTime = 0.0f;
		FVector FallingStartedPosition = FVector::ZeroVector;
	} FallDamage;
protected:
	//추락시 데미지 타입. (카메라 셰이킹, 컨트롤러 진동을 여기에 미리 설정해놓음)
	UPROPERTY(EditAnywhere, Category="FallOffDamage")
	TSubclassOf<UPBFallOffDamageType> FallOffDamageTypeClass;
	UPROPERTY(EditDefaultsOnly, Category = "FallOffDamage")
	UCurveFloat* FallOffCameraShakingScaleCurve;
	//데미지를 입기 시작하는 최소 속도
	UPROPERTY(EditAnywhere, Category = "FallOffDamage")
	float FallOffDamageSpeed;
	//속도당 받게되는 데미지 비율.
	UPROPERTY(EditAnywhere, Category = "FallOffDamage")
	float FallOffDamageSlope;
	//추락시 받는 최소 데미지.
	UPROPERTY(EditAnywhere, Category = "FallOffDamage")
	float FallOffDamageMin;
	//이 시간을 초과하는 동안 계속 떨어지는 중이면 사망처리.
	UPROPERTY(EditAnywhere, Category = "FallOffDamage")
	float FallOffDeathTime;
	UPROPERTY(BlueprintReadOnly, Category = "FallOffDamage")
	bool bIsFalling;
public:
	FTimerHandle TimerHandle_AimingTrace;
	FHitResult AimingTraceResultCache;
	UPROPERTY(Transient, Replicated)
	APBCharacter *AimingTargetCache;

	bool bIsAimingTargeted;

	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	//!+//@note: 현재 ActorComponent를 소유하는 경우 Editor에서 Crash발생함 (4.14.2에서 해결 되었다고 함)
	//UPROPERTY(VisibleDefaultsOnly, Category = DebugComponent)
	//class UPBDebugActorComponent *DebugComponent;

	/* ------------------------------------------------------------------------------- */
	// Movement States
	/* ------------------------------------------------------------------------------- */

protected:
	// Call anywhere, works globally
	void SetWalking(bool bNewWalking);
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetWalking(bool bNewWalking);

	UPROPERTY(Transient, Replicated)
	uint8 bWantsToWalk : 1;

	// Call anywhere, works globally
	void SetSprinting(bool bNewSprinting);
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetSprinting(bool bNewSprinting);

	UPROPERTY(Transient, Replicated)
	uint8 bWantsToSprint : 1;

	bool CanSprint() const;

public:
	UFUNCTION(BlueprintCallable, Category = Pawn)
	EPBMovementType GetWantsMovementType() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsWalking() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetRunningSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetWalkingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetSprintingSpeedModifier() const;

	/* ------------------------------------------------------------------------------- */
	// Looking Around
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LookAround)
	UCurveFloat* EyeSpeed_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LookAround)
	float JoystickDeadZone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LookAround)
	float ZoomEyeSpeedMult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LookAround)
	float NoZoomHorzEyeSpeedMult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LookAround)		
	float FiringEyeSpeedMult;

	// Only valid in local controller
	float CachedAimSensitivity;

protected:
	void InitLookingAround();
	void RegisterLookAroundHandlers();
	void StartUpLookAround();
	FDelegateHandle UserSettingChangedEvtHandle;
	void HandleUserSettingChanged();
	void ShutDownLookAround();
	void AdjustLookAroundInputSetting(APlayerController* NewController);
	void Turn(float Val);
	void TurnAtRate(float Val);
	void LookUp(float Val);
	void LookUpAtRate(float Val);

	/* ------------------------------------------------------------------------------- */
	// UnBeatable
	/* ------------------------------------------------------------------------------- */

public:
	// Call only on server
	void UnbeatableOn(float Period = 0);
	void UnbeatableOff();

	// Call anywhere
	bool HasUnbeatable();

protected:
	void InitUnBeatableSys();

	FTimerHandle TimerHandle_Unbeatable;

	UPROPERTY(Transient, Replicated)
	bool bUnbeatable;

	/* ------------------------------------------------------------------------------- */
	// Recoil
	/* ------------------------------------------------------------------------------- */

public:
	virtual FRotator GetViewRotation() const override;
	virtual FRotator GetBaseAimRotation() const override;

	void NotifyWeaponBurst(int BurstCounter, const FWeaponRecoilData& WeaponData);

protected:
	void InitRecoilSystem();
	void InterpRecoilCorr(float DeltaTime);

protected:
	FRandomStream RecoilRandomStream;

	// [Local]
	float RecoilPitchCorr;
	float RecoilYawCorr;

	// [Local]
	float InterpedRecoilPitchCorr;
	float InterpedRecoilYawCorr;

	/* ------------------------------------------------------------------------------- */
	// Detection
	/* ------------------------------------------------------------------------------- */

public:
	void RaiseDetectableEvent();
	float GetTimeSinceLastDetectableEvent();

protected:
	void InitDetection();
	void StartUpDetection();
	void ShutDownDetection();

	FTimerHandle CheckDetectable_Handle;
	void CheckDetectable();

	float LastDetectableEventRaisedAt;



public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FirstPerson ViewPoint")
	class USceneComponent *FirstPersonViewScene;

protected:

	void InitFirstPersonViewpoint();

	/* ------------------------------------------------------------------------------- */
	// Ragdoll ViewPoint
	/* ------------------------------------------------------------------------------- */

public:

	FVector GetRagdollViewpointPosition();

	FRotator GetRagdollViewpointRotation();

	/* ------------------------------------------------------------------------------- */
	// ThirdPerson ViewPoint
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ThirdPerson ViewPoint")
	class USpringArmComponent* ThirdPersonSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|SphericalCam")
	class USpringArmComponent* DebugSphericalCamSpringArm;

private:
	void UpdateSpectatorCameraRotationSync();
private:
	UPROPERTY(Transient, Replicated)
	FTransform FirstPersonViewTransform;
public:
	UFUNCTION(BlueprintCallable, Category = Camera)
	FTransform GetFirstPersonViewTransform() const;
private:
	UPROPERTY(Transient, Replicated)
	FTransform ThirdPersonViewTransform;
public:
	UFUNCTION(BlueprintCallable, Category = Camera)
	FTransform GetThirdPersonViewTransform() const;

protected:

	void InitThirdPersonViewpoint();
	
	void InitDebugCamViewpoint();
	
	FTransform SpringArmDefaultRelativeTransform;

	FVector SmoothedCapsuleLocation;
	FQuat SmoothedCapsuleRotation;

	/* ------------------------------------------------------------------------------- */
	// Camera
	/* ------------------------------------------------------------------------------- */

public:

	// A Camera component that helps artist visualization and give modifier settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ThirdPersonCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* DebugFreeCamComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* DebugSphericalCamComp;

private:
	void InitStartUpCamera();
protected:

	void InitCameraSys();
	void EnterRagdollCamMode(AActor *Target);
	void EnterDeathCamMode(AActor *Target, AActor* Killer);
	void EnterFPSCamMode(AActor *Target);
	void EnterTPSCamMode(AActor *Target);
	void EnterDebugFreeCamMode(AActor *Target);
	void EnterDebugSphericalCamMode(AActor *Target);

	/* ------------------------------------------------------------------------------- */
	// NameTag
	/* ------------------------------------------------------------------------------- */
	
protected:
	class UPBPlayerNameTag* GetNameTagWidget();

	// set this pointer to nametag widget
	void SetCharacterPtrToNameTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NameTag")
	class UWidgetComponent* NameTagWidgetComp;

	/* ------------------------------------------------------------------------------- */
	// ComponentDamage
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	float HeadShotDmgAdjust;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	float BodyShotDmgAdjust;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	float ArmShotDmgAdjust;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	float LegShotDmgAdjust;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	TArray<FName> HeadBones;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	TArray<FName> BodyBones;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	TArray<FName> ArmBones;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ComponentDamage")
	TArray<FName> LegBones;

public:
	bool IsHeadShot(FDamageEvent const& DamageEvent);
	bool IsBodyShot(FDamageEvent const& DamageEvent);
	bool IsArmShot(FDamageEvent const& DamageEvent);
	bool IsLegShot(FDamageEvent const& DamageEvent);
	bool IsFallOff(FDamageEvent const& DamageEvent);

protected:
	void InitComponentDamageSys();
	float ModifyDamageByHitComp(float OriginalDamage, FDamageEvent const& DamageEvent, AController* InstigatorController);

	/* ------------------------------------------------------------------------------- */
	// Back stab
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BackstabDamage")
	float MaxBackstabDamage;

public:
	float GetBackstabDmgAdjust(FDamageEvent const& DamageEvent);

protected:
	void InitBackstabDamageSys();
	float ModifyDamageForBackstab(float OriginalDamage, FDamageEvent const& DamageEvent, AController* InstigatorController);

	/* ------------------------------------------------------------------------------- */
	// Death Impulse
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
	FName DefaultImpulseReceivingBone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
	TArray<FBoneImpulseReceptor> ImpulseReceivingBones;

protected:
	void InitDeathImpulse();
	void ProcessDeathRagdollImpulse();

	/* ------------------------------------------------------------------------------- */
	// Mission Utilities
	/* ------------------------------------------------------------------------------- */

	APBMission* GetOwnMission();

	/* ------------------------------------------------------------------------------- */
	// Stash Weapon for Bomb planting and Bomb defusing to swap back later
	/* ------------------------------------------------------------------------------- */

protected:

	// Only valid in local controller
	EWeaponSlot StashedWeaponSlot;
	int32 StashedWeaponSlotIdx;
	EWeaponMode StashedWeaponMode;

	/* ------------------------------------------------------------------------------- */
	// Bomb Planting
	/* ------------------------------------------------------------------------------- */

public:

	// Call on server
	void NotifyEnterBombSite(const class APBTrigger_BombSite *BombSite);
	void NotifyLeaveBombSite();

	// Valid everywhere
	UFUNCTION(BlueprintCallable, Category = Bomb)
	bool AimToPlantBomb();
	bool CanPlantBomb();
	bool IsInTimePlantableBomb();
	float GetBombPlantingTime();

	// Call on local controller
	void OnStartBombPlanting();
	void OnEndBombPlanting();

	// Valid only in local controller
	bool IsPlantingBomb();
	void OnStartPlantBomb();
	void OnEndPlantBomb();

protected:

	void InitBombPlanting();

	APBWeapon* FindBombFromInventory();
	void SwitchToBombWeapon();

	FTimerHandle Handle_UpdateBombPlanting;
	void UpdateBombPlanting();

	UPROPERTY(Transient, ReplicatedUsing=OnRep_BombsitePresenceCount)
	int BombsitePresenceCount;
	UFUNCTION()
	void OnRep_BombsitePresenceCount(int OriginalValue);
	
	// Only valid in local controller
	bool bIsPlantingBomb;

protected:
	// For G.C.
	UPROPERTY()
	const APBTrigger_BombSite *OverlappedBombSite;
public:
	const APBTrigger_BombSite *GetOverlappedBombSite() const;

	/* ------------------------------------------------------------------------------- */
	// Bomb Defusing
	/* ------------------------------------------------------------------------------- */

public:

	// Called on server only
	void NotifyEnterBombRadius(class APBProj_Bomb* Bomb);
	void NotifyLeaveBombRadius(APBProj_Bomb* Bomb);

	// Valid everywhere
	UFUNCTION(BlueprintCallable, Category = Bomb)
	bool AimToDefuseBomb();
	bool CanDefuseBomb();
	float GetBombDefusingTime();

	// Call on local controller
	void OnStartBombDefusing();
	void OnEndBombDefusing();

	// Valid only in local controller
	bool IsDefusingBomb();
	void OnStartDefuseBomb();
	void OnEndDefuseBomb();

protected:

	void InitBombDefusing();

	APBWeapon* FindDefuseKitFromInventory();
	void SwitchToDefuseKitWeapon();

	FTimerHandle Handle_UpdateBombDefusing;
	void UpdateBombDefusing();

	// Only valid in local controller
	bool bIsDefusingBomb;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */

protected:

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = InventoryInEditor)
	TArray<FWeaponModeSetClass> PrimaryWeaponClasses;

	UPROPERTY(EditDefaultsOnly, Category = InventoryInEditor)
	TArray<FWeaponModeSetClass> SecondaryWeaponClasses;

	UPROPERTY(EditDefaultsOnly, Category = InventoryInEditor)
	TArray<FWeaponModeSetClass> MeleeWeaponClasses;

	UPROPERTY(EditDefaultsOnly, Category = InventoryInEditor)
	TArray<FWeaponModeSetClass> GrenadeWeaponClasses;

	UPROPERTY(EditDefaultsOnly, Category = InventoryInEditor)
	TArray<FWeaponModeSetClass> SpecialWeaponClasses;

	/** weapons in inventory */
	UPROPERTY(Transient, Replicated)
	TArray<FWeaponSlotSet> Inventory;

	EWeaponSlot CurrentWeaponSlot;
	int32		CurrentWeaponSlotIdx;
	EWeaponMode CurrentWeaponMode;
	TArray<FWeaponModeSet> EmptySlotWeapons;

	int32 CurrentGrenadeSlotIdx;

	// If the player pressed the quick throw button, then the weapon that is bound to here would be fired.
	EWeaponSlot CurrentSelectedQuickWeapSlot;


	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentWeapon)
	class APBWeapon* CurrentWeapon;

	//Quick throw, Quick knife 등에 사용한다 
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentQuickWeapon)
	class APBWeapon* CurrentQuickWeapon;

	/** current Zoom state */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ZoomState)
	uint8 bIsZooming : 1;	

	bool bWantsToZooming;

	/** current firing state */
	uint8 bWantsToFireTrigger[2];
	bool bWantsToMeleeTrigger;
	bool bWantsToQuickThrowing;




	/** when low health effects should start */
	float LowHealthPercentage;

	/** material instances for setting team color in mesh (3rd person view) */
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> MeshMIDs;

	/** animation played on death */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* DeathAnim;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	TArray<FCharacterVoice> Voices;

	UPROPERTY(Transient)
	TMap<EPBVoiceType, USoundCue*> VoiceMap;

	/** sound played on death, local player only */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* DeathSound;

	/** effect played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	UParticleSystem* RespawnFX;

	/** sound played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* RespawnSound;

	/** sound played when health is low */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* LowHealthSound;

	/** sound played when running */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* RunLoopSound;

	/** sound played when stop running */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* RunStopSound;

	/** sound played when zooming state changes */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ZoomingSound;

	/** sound played when zooming state changes */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* BombPlantActionSound;
	UAudioComponent *BombPlantActionComponent;

	/** sound played when zooming state changes */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* BombDefuseActionSound;
	UAudioComponent *BombDefuseActionComponent;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* MissionStartSound;

	/** used to manipulate with run loop sound */
	UPROPERTY()
	UAudioComponent* RunLoopAC;

	/** hook to looped low health sound used to stop/adjust volume */
	UPROPERTY()
	UAudioComponent* LowHealthWarningPlayer;

	/** impact effects */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> FootstepEffect1P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> FootstepEffect3P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> JumpEffect1P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> JumpEffect3P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> LandEffect1P;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> LandEffect3P;

public:
	void PlaySoundMissionStart();

protected:
	/** handles sounds for running */
	void UpdateSprintSounds(bool bNewSprinting);

	/** handle mesh colors on specified material instance */
	void UpdateTeamColors(UMaterialInstanceDynamic* UseMID);

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCharacterData CharacterData;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayAnimation)
	FPBPlayAnimData SyncPlayAnimData;

	UFUNCTION()
	void OnRep_PlayAnimation();
	//////////////////////////////////////////////////////////////////////////
	// Damage & death

public:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float DamagedSpeedModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health)
	uint32 bIsDying:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint32 bSpiritLeftBody : 1;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_bTriedBecomeSpectatorOnAlive)
	uint32 bTriedBecomeSpectatorOnAlive : 1;
	UFUNCTION()
	void OnRep_bTriedBecomeSpectatorOnAlive();


	UPROPERTY(Transient, Replicated)
	bool bIsViewOwnself;

	UPROPERTY(Transient, Replicated)
	EPBCameraStyle CurrentCameraStyle;

	/** Pawn suicide */
	virtual void Suicide();

	/** Kill this pawn */
	virtual void KilledBy(class APawn* EventInstigator);

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** [Server Only] Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void OnApplyDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/**
	 * Kills pawn.  Server/authority only.
	 * @param KillingDamage - Damage amount of the killing blow
	 * @param DamageEvent - Damage event of the killing blow
	 * @param Killer - Who killed this pawn
	 * @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	 * @returns true if allowed
	 */
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

protected:

	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	void PlayFeedbackEffects(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = Pawn)
	void PlayHitEffects();	

	/** [local] spawn hit crosshair of EventInstigator */
	void SpawnHitCrosshair(class APawn* EventInstigator, bool bIsKilledByHit);
	void SpawnHitcrosshair(bool bIsKilledByHit);

	/** Replicate where this pawn was last hit and damaged */
	UPROPERTY(BlueprintReadWrite, Category = "Pawn", Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	void BecomeRagDoll();
	void BecomeSpectatorOnAlive();
public:
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerBecomeSpectatorOnAlive();
protected:
	void SpiritLeaveBody();
	void BecomeFollower();
	FTimerHandle UpdateFollowerTimerHandle;
	void OnFollowPreviousPlayer();
	void OnFollowNextPlayer();
	void UpdateFollowTargetOnSpectatorMode();
	// bIsToGetNextPlayer가 false이면 이전 플레이어를 Follow 시도한다.
	void FollowPlayer(bool bTryToGetNextPlayer, float InDelay);
	void FollowPreviousPlayer(float InDelay);
	void FollowNextPlayer(float InDelay);

	UPROPERTY(Transient, Replicated)
	APBPlayerState *TargetViewPlayer;
	APBPlayerState *CandidatedFollowPlayer;
public:
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	APBPlayerState *GetTargetViewPlayer() const;

protected:
	APBPlayerState *GetPlayerStateToFollow(APBPlayerState *(*InFunctionAsIncreaser)(const TArray<APBPlayerState *> &, APBPlayerState *const &));
	APBPlayerState *DecideNextPlayerToFollow();
	APBPlayerState *DecidePreviousPlayerToFollow();

	virtual void Respawn();

public:
	//////////////////////////////////////////////////////////////////////////
	// helper
	FVector GetBoneLocation(FName BoneName);
	FQuat GetBoneQuaternion(FName BoneName);

	//angle range (-180, 180)
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetHitAngle(const FVector& AttackPos);

	//angle range (-180, 180)
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetMoveDirectionAngle() const;

	//angle range (-180, 180), between ForwardVector and Direction2
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetBetweenAngleForwardAnd(FVector Direction2) const;

	//angle range (-180, 180)
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetBetweenAngle(FVector Direction1, FVector Direction2) const;

	// GetVelocity(X,Y,Z)
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetSpeed() const;
	//Horizontal speed (GetVelocity(X,Y))
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetSpeed_H() const;
	//Vertical speed (GetVelocity(Z))
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetSpeed_V() const;

	//Standing (GetSpeed_H() == 0 and Acceleration == 0)
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsStandingStill() const;

	/* ------------------------------------------------------------------------------- */
	// Related with kill mark
	/* ------------------------------------------------------------------------------- */

	// [only server called]
	void ForgetWhoKilledMe();

};


