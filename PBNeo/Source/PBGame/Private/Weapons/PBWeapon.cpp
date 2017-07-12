// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Weapons/PBWeapon.h"
#include "Weapons/Action/PBWeapAction.h"
#include "Weapons/Attachment/PBWeapAttachment.h"
#include "Weapons/Bullet/PBInst_MeleeBullet.h"
#include "Weapons/PBWeapFanTrace.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effects/PBImpactEffect.h"
#include "Bots/PBAIController.h"
#include "Player/PBPlayerState.h"
#include "Table/Item/PBItemTableManager.h"
#include "PBGameplayStatics.h"
#include "PBGameInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "PBWeapActionQuickThrow.h"
#include "PBGameViewportClient.h"

APBWeapon::APBWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent, USceneComponent>(this, TEXT("DummyRoot"), false);
	//MeleeAttack.HitTrace = ObjectInitializer.CreateDefaultSubobject<UPBWeapFanTrace>(this, TEXT("WeaponMeleeTrace"), false);
	bIsEquipped = false;
	bWantsToFireReady.SetNum(2);
	bWantsToFireReady[0] = false;
	bWantsToFireReady[1] = false;
	bWantsToFire.SetNum(2);
	bWantsToFire[0] = false;
	bWantsToFire[1] = false;
	bWantsToMeleeAttack.SetNum(2);
	bWantsToMeleeAttack[0] = false;
	bWantsToMeleeAttack[1] = false;

	bPendingReload = false;
	bPendingEquip = false;
	PrevState = EWeaponState::Idle;
	CurrentState = EWeaponState::Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	LastFireTime = 0.0f;
	LastMeleeAttackTime = 0.0f;

	CurrentFiringDeviation = 0.f;
	CurrentExpansionSize = 0.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	WeaponType = EWeaponType::AR;
	WeaponStance = EWeaponStance::Rifle;

	UseTableDataToConfig = true;

	bPendingFiring = false;
	bPostFiring = false;	
	bPostMeleeAttack = false;
	RefTableData = nullptr;

	bUsePierceWeapon = false;
}

void APBWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeapAttachment)
	{
		WeapAttachment->Init();
	}
	
	CreateBullets();

	CurrentFiringDeviation = WeaponDeviationConfig.BaseDeviation;
	CurrentExpansionSize = WeaponCrossHairConfig.BaseExpansion;

	FillAmmo();

	DetachMeshFromPawn();
}

void APBWeapon::Tick(float DeltaSeconds)
{
	UpdateBurstEffect(DeltaSeconds);
	UpdateDeviation(DeltaSeconds);
}

bool APBWeapon::ApplyTableData(const FPBWeaponTableData* Data)
{
	if (!GetUseTableTata())
	{
		return false;
	}

	bool bIsDirty = false;

	// Weapon
	bIsDirty = WeaponConfig.ImportTableData(Data, bIsDirty);
	bIsDirty = WeaponRecoilConfig.ImportTableData(Data, bIsDirty);
	bIsDirty = WeaponDeviationConfig.ImportTableData(Data, bIsDirty);
	bIsDirty = WeaponCrossHairConfig.ImportTableData(Data, bIsDirty);

	// Bullet or Projectile
	if (WeapAttachment)
	{
		bool bTempIsDirty = false;

		WeapAttachment->Init();
		bTempIsDirty = WeapAttachment->ApplyTableData(Data);
		
		if (!bIsDirty)
		{
			bIsDirty = bTempIsDirty;
		}

		WeapAttachment->Empty();
	}

	return bIsDirty;
}

bool APBWeapon::GetUseTableTata()
{
	return UseTableDataToConfig && GetItemID() > 0;
}

FPBWeaponTableData* APBWeapon::GetTableData()
{
	if (RefTableData)
	{
		return RefTableData;
	}

	auto TM = UPBItemTableManager::Get(this);
	RefTableData = TM ? TM->GetWeaponTableData(GetItemID()) : nullptr;
	return RefTableData;
}

float APBWeapon::ApplyMovementSpeedModifier(float Speed)
{
	// weapon 
	Speed += Speed * WeaponConfig.MovementSpeedModifierBase;

	// on firing
	Speed += Speed * (bPostFiring ? WeaponConfig.MovementSpeedModifierOnFire : 0.0f);

	return Speed;
}

void APBWeapon::Destroyed()
{
	Super::Destroyed();

	StopWeaponFireEffects();
}

// 무기 메쉬의 show, hide 는 이 함수 안에서만 처리한다. 외부에서 SetHiddenInGame 를 호출하지 말것.
void APBWeapon::UpdateMeshVisibilitys(bool bForce)
{
	if (WeapAttachment)
	{
		WeapAttachment->UpdateMeshVisibilitys(bForce);
	}
}

void APBWeapon::ForceMeshVisibilitys(bool bVisible, bool bFirstPerson)
{
	if (WeapAttachment)
	{
		WeapAttachment->ForceMeshVisibilitys(bVisible, bFirstPerson);
	}
}

APBBulletBase* APBWeapon::SpawnBullet(EWeaponPoint ePoint, const FTransform& Transform)
{
	if (WeapAttachment)
	{
		return WeapAttachment->SpawnBullet(ePoint, Transform);
	}
	return nullptr;
}

void APBWeapon::CreateBullets()
{
	if (WeapAttachment)
	{
		for (int32 i = 0; i < WeapAttachment->GetAttachmentDataCount(); ++i)
		{
			WeapAttachment->SpawnMeleeBullet(EWeaponPoint(i), FTransform::Identity);
		}
	}
}

APBInst_MeleeBullet* APBWeapon::GetMeleeBullet() const
{
	if (WeapAttachment)
	{
		return Cast<APBInst_MeleeBullet>(WeapAttachment->GetMeleeBullet(FiringSync.FirePoint));
	}
	return nullptr;
}

EWeaponType APBWeapon::GetWeaponType() const
{
	return WeaponType;
}

void APBWeapon::SetWeaponType(EWeaponType type)
{
	WeaponType = type;
}

void APBWeapon::SetItemID(int32 ID)
{
	WeaponConfig.ItemID = ID;
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void APBWeapon::OnEquip(const APBWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	bPendingFiring = false;
	bPendingReload = false;
	bPostFiring = false;
	bPostMeleeAttack = false;
	DetermineWeaponStates();

	// Only play animation if last weapon is valid
	if (LastWeapon)
	{		
		float AnimDuration = PlayWeaponAnimation(EWeaponAnimType::Equip);

		if (WeaponConfig.EquipDuration <= 0.f)
		{
			const float MinimumAnimationLength = 0.5f;
			WeaponConfig.EquipDuration = FMath::Max(AnimDuration, MinimumAnimationLength);
		}

		EquipStartedTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

		if (WeaponConfig.EquipDuration <= 0.f)
		{
			OnEquipFinished();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &APBWeapon::OnEquipFinished, WeaponConfig.EquipDuration, false);
		}

	}
	else
	{
		OnEquipFinished();
	}

	if (MyPawn)
	{
		//if (MyPawn->IsFirstPersonView())
		if (GetViewerIsFirstPerson())
		{
			PlayWeaponSounds(EWeaponSoundType::Equip);
		}

		MyPawn->NotifyEquipWeapon(this);
		
	}

	if (Role == ROLE_Authority)
	{
		PlayEquipCameraShake();
	}
}

void APBWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;
	bPostMeleeAttack = false;
	bWantsToFire[0] = false;
	bWantsToFire[1] = false;
	bWantsToFireReady[0] = false;
	bWantsToFireReady[1] = false;
	bWantsToMeleeAttack[0] = false;
	bWantsToMeleeAttack[1] = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponStates();
	
	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() &&
			CurrentAmmoInClip <= 0 &&
			CanReload())
		{
			StartReload();
			return;
		}

		// if Pawn's WantsToFire == true, start firing
		if (MyPawn->IsLocallyControlled())
		{
			WakeFireTriggers();
			MyPawn->OnEquipFinished();
		}
	}	
}

void APBWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	EndFire(EFireTrigger::Fire1);
	EndFire(EFireTrigger::Fire2);

	//melee attack end
	SetWeaponAttackState(EFireTrigger::Fire1, false);
	SetWeaponAttackState(EFireTrigger::Fire2, false);

	if (MyPawn && MyPawn->IsZooming())
	{
		MyPawn->SetZooming(false);
	}

	if (bPendingReload)
	{
		StopWeaponAnimation(EWeaponAnimType::Reload);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EWeaponAnimType::Equip);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	StopEquipCameraShake();

	DetermineWeaponStates();
}

void APBWeapon::OnEnterInventory(APBCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void APBWeapon::OnLeaveInventory()
{
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}
}

void APBWeapon::AttachMeshToPawn()
{
	if (WeapAttachment)
	{
		WeapAttachment->AttachMeshToPawn();
		WeapAttachment->UpdateMeshVisibilitys(true);
	}
}

void APBWeapon::DetachMeshFromPawn()
{
	if (WeapAttachment)
	{
		WeapAttachment->DetachMeshFromPawn();
		//WeapAttachment->UpdateMeshVisibilitys(false);
	}
}

void APBWeapon::PlayFireForceFeedback(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->PlayFireForceFeedback(ePoint);
	}
}

void APBWeapon::PlayFireCameraShake(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->PlayFireCameraShake(ePoint);
	}
}

void APBWeapon::PlayEquipCameraShake(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->PlayEquipCameraShake(ePoint);
	}
}

void APBWeapon::StopEquipCameraShake(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->StopEquipCameraShake(ePoint);
	}
}

void APBWeapon::PlayReloadCameraShake(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->PlayReloadCameraShake(ePoint);
	}
}

void APBWeapon::StopReloadCameraShake(EWeaponPoint ePoint)
{
	if (WeapAttachment)
	{
		WeapAttachment->StopReloadCameraShake(ePoint);
	}
}

bool APBWeapon::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = HasAmmo() && (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

bool APBWeapon::CanZooming() const
{
	return (MyPawn != nullptr && IsReloading() == false && IsEquiping() == false);
}

void APBWeapon::StartReload()
{
	if (Role < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (CanReload())
	{
		OnStartReload();
	}
}

void APBWeapon::OnStartReload()
{
	bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	bPendingReload = true;
	DetermineWeaponStates();

	if (MyPawn && MyPawn->IsZooming())
	{
		MyPawn->SetZooming(false);
	}
	
	float AnimDuration = PlayWeaponAnimation(EWeaponAnimType::Reload);

	// side effect : 1,3인칭 애니메이션 시간을 통일 하지 않으면 장전이 완료되고 한번 더 하는 일이 발생할 수 있다.
	// 1,3인칭 길이를 동일하게 맞추던가, 아니면 정해놓은 값을 사용해야한다.
	
	if (WeaponConfig.ReloadDuration <= 0.0f)
	{
		const float MinimumAnimationLength = 0.5f;
		WeaponConfig.ReloadDuration = FMath::Max(AnimDuration, MinimumAnimationLength);
	}

	if (Role == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &APBWeapon::StopReload, WeaponConfig.ReloadDuration, false);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &APBWeapon::ReloadWeapon, FMath::Max(0.1f, WeaponConfig.ReloadDuration - 0.1f), false);

		PlayReloadCameraShake();
	}

	if (bLocallyControlled)
	{
		PlayWeaponSounds(EWeaponSoundType::Reload);
	}
}

void APBWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		StopWeaponAnimation(EWeaponAnimType::Reload);
		DetermineWeaponStates();		

		// if Pawn's WantsToFire == true, start firing
		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			WakeFireTriggers();
			MyPawn->OnReloadEnded();
		}

		StopReloadCameraShake();
	}
}

void APBWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}

	if (HasInfiniteClip())
	{
		CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}
}

bool APBWeapon::ServerStartReload_Validate()
{
	return true;
}

void APBWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool APBWeapon::ServerStopReload_Validate()
{
	return true;
}

void APBWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

void APBWeapon::ClientStartReload_Implementation()
{
	StartReload();
}

//////////////////////////////////////////////////////////////////////////
// Control


void APBWeapon::NotifyWeaponBurstRecoil(int BurstCounter)
{
	auto PBC = Cast<APBCharacter>(MyPawn);
	if (PBC)
	{
		// Only need local notification
		PBC->NotifyWeaponBurst(BurstCounter, WeaponRecoilConfig);
	}
}

float APBWeapon::GetShootingDeviation()
{
	if (FiringSync.BurstCounter < WeaponDeviationConfig.DeviationBeginAtShot - 1)
	{
		return 0.f;
	}
	else
	{
		return GetCurrentDeviation();
	}
}

float APBWeapon::GetCurrentDeviation()
{
	return CurrentFiringDeviation * CurrentDeviationMultiplier;
}

void APBWeapon::AddDeviationOnFire()
{
	if (FiringSync.BurstCounter >= WeaponDeviationConfig.DeviationBeginAtShot - 1)
	{
		CurrentFiringDeviation += WeaponDeviationConfig.DeviationAddRate_Fire;
		CurrentFiringDeviation = FMath::Clamp(CurrentFiringDeviation, 0.f, WeaponDeviationConfig.MaxDeviation);
	}
}

void APBWeapon::UpdateDeviation(float Delta)
{
	float DeviationReturnRate = UPBGameplayStatics::InterpDelayToInterpSpeed((GetCurrentState() == EWeaponState::Firing) ? 
		WeaponDeviationConfig.DeviationReturnDelay_Firing : WeaponDeviationConfig.DeviationReturnDelay_Normal);
	if (DeviationReturnRate > SMALL_NUMBER)
	{
		CurrentFiringDeviation = FMath::FInterpTo(CurrentFiringDeviation, WeaponDeviationConfig.BaseDeviation, Delta, DeviationReturnRate);
	}

	float TargetMultiplier = 1.f;
	if (MyPawn)
	{
		if (MyPawn->IsZooming())
		{
			TargetMultiplier *= (1 + WeaponDeviationConfig.ZoomDeviationModifier);
		}

		if (MyPawn->GetCharacterMovement() && MyPawn->GetCharacterMovement()->Velocity.Z > 1)
		{
			TargetMultiplier *= (1 + WeaponDeviationConfig.DeviationRateJump);
		}
		else if (MyPawn->IsCrouching())
		{
			TargetMultiplier *= (1 + WeaponDeviationConfig.DeviationRateCrouch);
		}
		else if (MyPawn->GetWantsMovementType() == EPBMovementType::Sprinting)
		{
			TargetMultiplier *= (1 + WeaponDeviationConfig.DeviationRateSprint);
		}
		else if (MyPawn->GetWantsMovementType() != EPBMovementType::Standing)
		{
			// Walking or Running
			TargetMultiplier *= (1 + WeaponDeviationConfig.DeviationRateWalk);
		}
	}

	static const float MultiplierInterpRate = 5.f;
	CurrentDeviationMultiplier = FMath::FInterpTo(CurrentDeviationMultiplier, TargetMultiplier, Delta, MultiplierInterpRate);
}

float APBWeapon::GetExpansionSize()
{
	return CurrentExpansionSize * CurrentExpansionMultiplier;
}

void APBWeapon::AddBurstEffectOnFire()
{
	if (FiringSync.BurstCounter >= WeaponCrossHairConfig.ExpansionBeginAtShot - 1)
	{
		CurrentExpansionSize += WeaponCrossHairConfig.ExpansionAddRate_Fire;
		CurrentExpansionSize = FMath::Clamp(CurrentExpansionSize, 0.f, WeaponCrossHairConfig.MaxExpansion);
	}
}

void APBWeapon::UpdateBurstEffect(float Delta)
{
	float ExpansionReturnRate = UPBGameplayStatics::InterpDelayToInterpSpeed((GetCurrentState() == EWeaponState::Firing) ? 
		WeaponCrossHairConfig.ExpansionReturnDelay_Firing : WeaponCrossHairConfig.ExpansionReturnDelay_Normal);
	if (ExpansionReturnRate > SMALL_NUMBER)
	{
		CurrentExpansionSize = FMath::FInterpTo(CurrentExpansionSize, WeaponCrossHairConfig.BaseExpansion, Delta, ExpansionReturnRate);
	}

	float TargetMultiplier = 1.f;
	if (MyPawn)
	{
		if (MyPawn->IsZooming())
		{
			TargetMultiplier *= (1 + WeaponCrossHairConfig.ZoomExpansionModifier);
		}

		if (MyPawn->GetCharacterMovement() && MyPawn->GetCharacterMovement()->Velocity.Z > 1)
		{
			TargetMultiplier *= (1 + WeaponCrossHairConfig.ExpansionRateJump);
		}
		else if (MyPawn->IsCrouching())
		{
			TargetMultiplier *= (1 + WeaponCrossHairConfig.ExpansionRateCrouch);
		}
		else if (MyPawn->GetWantsMovementType() == EPBMovementType::Sprinting)
		{
			TargetMultiplier *= (1 + WeaponCrossHairConfig.ExpansionRateSprint);
		}
		else if (MyPawn->GetWantsMovementType() != EPBMovementType::Standing)
		{
			// Walking or Running
			TargetMultiplier *= (1 + WeaponCrossHairConfig.ExpansionRateWalk);
		}
	}

	static const float MultiplierInterpRate = 5.f;
	CurrentExpansionMultiplier = FMath::FInterpTo(CurrentExpansionMultiplier, TargetMultiplier, Delta, MultiplierInterpRate);
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void APBWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	APBAIController* BotAI = MyPawn ? Cast<APBAIController>(MyPawn->GetController()) : NULL;
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}
	
	// start reload if clip was empty
	if (GetCurrentAmmoInClip() <= 0 &&
		CanReload() &&
		MyPawn->GetCurrentWeapon() == this)
	{
		ClientStartReload();
	}
}

bool APBWeapon::HasAmmoInClip() const
{
	return (CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo());
}

bool APBWeapon::HasAmmo() const
{
	return (CurrentAmmo > 0 || HasInfiniteClip() || HasInfiniteAmmo());
}

void APBWeapon::FillAmmo()
{
	CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
	CurrentAmmo = WeaponConfig.MaxAmmo;
}

void APBWeapon::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	APBAIController* BotAI = MyPawn ? Cast<APBAIController>(MyPawn->GetController()) : NULL;	
	APBPlayerController* PlayerController = MyPawn ? Cast<APBPlayerController>(MyPawn->GetController()) : NULL;
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}
}

void APBWeapon::UpdateFirstFiringFlag()
{
	if (Role == ROLE_Authority)
	{
		if (MyPawn)
		{
			auto ps = Cast<APBPlayerState>(MyPawn->PlayerState);
			if (ps)
			{
				if (ps->GetKillMarkProperties().FiringCnt < 2)
				{
					ps->GetKillMarkProperties().FiringCnt++;
				}
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Firing start

void APBWeapon::PendingFiring(float PendingFire)
{	
	bPendingFiring = true;
	GetWorldTimerManager().SetTimer(TimerHandle_ReFiring, this, &APBWeapon::OnFiring, PendingFire, false);
}

bool APBWeapon::IsInFireInterval() const
{
	const float GameTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	
	if (LastFireTime > 0 && WeaponConfig.FireInterval > 0.0f &&
		LastFireTime + WeaponConfig.FireInterval > GameTime)
	{
		return true;
	}
	return false;
}

void APBWeapon::FireReady(EFireTrigger eFireTrigger)
{
}

void APBWeapon::FireTriggerBegin(EFireTrigger eFireTrigger)
{	
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (Role < ROLE_Authority && bLocallyControlled)
	{
		ServerFireTriggerBegin(eFireTrigger);
	}

	UPBWeapAction* TriggerAction = (eFireTrigger == EFireTrigger::Fire1) ? FireTriggerAction1 : FireTriggerAction2;
	if (TriggerAction)
	{
		TriggerAction->BeginState(eFireTrigger);
	}
}

void APBWeapon::FireTriggerEnd(EFireTrigger eFireTrigger)
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled && Role < ROLE_Authority)
	{
		ServerFireTriggerEnd(eFireTrigger);
	}

	UPBWeapAction* TriggerAction = (eFireTrigger == EFireTrigger::Fire1) ? FireTriggerAction1 : FireTriggerAction2;
	if (TriggerAction)
	{
		TriggerAction->EndState(eFireTrigger);
	}
}

bool APBWeapon::ServerFireTriggerBegin_Validate(EFireTrigger eFireTrigger)
{
	return true;
}

void APBWeapon::ServerFireTriggerBegin_Implementation(EFireTrigger eFireTrigger)
{
	FireTriggerBegin(eFireTrigger);
}

bool APBWeapon::ServerFireTriggerEnd_Validate(EFireTrigger eFireTrigger)
{
	return true;
}

void APBWeapon::ServerFireTriggerEnd_Implementation(EFireTrigger eFireTrigger)
{
	FireTriggerEnd(eFireTrigger);
}

void APBWeapon::DetermineWeaponState(EFireTrigger eFireTrigger)
{
	EWeaponState NewState = EWeaponState::Idle;
	const int32 FireMode = (eFireTrigger == EFireTrigger::Fire1) ? 0 : 1;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}	
		else if (bWantsToFireReady[FireMode] && !IsInFireInterval() && CanFire() && !bPostFiring)
		{
			NewState = EWeaponState::FireReady;
		}
		else if (bWantsToMeleeAttack[FireMode] && CanMeleeAttack())
		{
			NewState = EWeaponState::MeleeAttack;
		}
		else if (bWantsToFire[FireMode] && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void APBWeapon::DetermineWeaponStates()
{
	DetermineWeaponState(EFireTrigger::Fire1);
	DetermineWeaponState(EFireTrigger::Fire2);
}

void APBWeapon::WakeFireTriggers()
{
	bWantsToMeleeAttack[0] = false;
	bWantsToMeleeAttack[1] = false;
	bWantsToFire[0] = false;
	bWantsToFire[1] = false;

	if (MyPawn)
		MyPawn->WakeFireTrigger();
}

void APBWeapon::SetWeaponState(EWeaponState NewState)
{
	PrevState = CurrentState;
	CurrentState = NewState;

	if (PrevState != EWeaponState::MeleeAttack && CurrentState == EWeaponState::MeleeAttack)
	{
		OnMeleeAttackStarted();
	}

	if (PrevState == EWeaponState::MeleeAttack && CurrentState != EWeaponState::MeleeAttack)
	{
		OnMeleeAttackFinished();
	}

	if (PrevState != EWeaponState::Firing && CurrentState == EWeaponState::Firing)
	{
		OnFireStarted(DesiredBurstCounter);
	}

	if (PrevState == EWeaponState::Firing && CurrentState != EWeaponState::Firing)
	{
		OnFireFinished();
	}
}

void APBWeapon::SetWeaponFireState(EFireTrigger eFireTrigger, bool bFireBegin)
{
	const int32 idx = (eFireTrigger == EFireTrigger::Fire1) ? 0 : 1;

	if (bWantsToFire[idx] != bFireBegin)
	{
		bWantsToFire[idx] = bFireBegin;
		DetermineWeaponState(eFireTrigger);
	}
}

void APBWeapon::SetWeaponFireReadyState(EFireTrigger eFireTrigger, bool bReady)
{
	const int32 idx = (eFireTrigger == EFireTrigger::Fire1) ? 0 : 1;

	if (bWantsToFireReady[idx] != bReady)
	{
		bWantsToFireReady[idx] = bReady;
		DetermineWeaponState(eFireTrigger);
	}	
}

void APBWeapon::SetWeaponAttackState(EFireTrigger eFireTrigger, bool bFireBegin)
{
	const int32 idx = (eFireTrigger == EFireTrigger::Fire1) ? 0 : 1;

	if (bWantsToMeleeAttack[idx] != bFireBegin)
	{
		bWantsToMeleeAttack[idx] = bFireBegin;
		DetermineWeaponState(eFireTrigger);
	}
}

// 재정의만 허용함 
void APBWeapon::StartFire(EFireTrigger eFireTrigger, int32 BurstCount)
{
	DesiredBurstCounter = BurstCount;

	SetWeaponFireState(eFireTrigger, true);
}

// 재정의만 허용함
void APBWeapon::EndFire(EFireTrigger eFireTrigger)
{
	SetWeaponFireState(eFireTrigger, false);
}

// 재정의만 허용함 
void APBWeapon::OnFireStarted(int32 BurstCount)
{
	DesiredBurstCounter = BurstCount;

	// start firing, can be delayed to satisfy FireInterval
	const float GameTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (IsInFireInterval())
		{
			const float ElapsedTime = SinceFiring();
			const float PendingFire = WeaponConfig.FireInterval - ElapsedTime;
			PendingFiring(PendingFire);
		}
		else
		{
			OnFiring();
		}
	}
}

// 재정의만 허용함 
void APBWeapon::OnFireFinished()
{
	// stop firing FX on remote clients
	FiringSync.BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopWeaponFireEffects();
	}	

	GetWorldTimerManager().ClearTimer(TimerHandle_ReFiring);
	GetWorldTimerManager().ClearTimer(TimerHandle_PostFiringEnd);
	bRefiring = false;
	bPendingFiring = false;
	bPostFiring = false;
}

bool APBWeapon::CanEquip() const
{
	return (!IsEquiping() && !IsEquipped());
}

bool APBWeapon::CanFire() const
{
	bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();
	bool bCanFire = MyPawn && MyPawn->CanFire();
	if (!bCanFire)
		return false;
	
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing) || (CurrentState == EWeaponState::FireReady));
	if (!bStateOKToFire)
		return false;

	if (bLocallyControlled && IsMeleeAttacking())
		return false;

	return !bPendingReload;
}

bool APBWeapon::CanMeleeAttack() const
{
	bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();
	bool bCanFire = MyPawn && MyPawn->CanFire();
	if (!bCanFire)
		return false;

	bool bStateOKToAttack = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::MeleeAttack));
	if (!bStateOKToAttack)
		return false;

	if (bLocallyControlled)
	{
		const float GameTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const float AttackInterval = WeaponConfig.MeleeAttackInterval;
		if (LastMeleeAttackTime > 0 && AttackInterval > 0.0f &&	LastMeleeAttackTime + AttackInterval > GameTime)
		{
			return false;
		}
	}

	return true;
}

void APBWeapon::PreFiring(EWeaponPoint ePoint)
{
	bPendingFiring = true;
	bPostFiring = false;
}

// 다른 곳에서 호출 하지마시오. 재정의만 허용함 
// 최종 발사처리(only Local/server), 양손 동시에 발사하는 경우 연속으로 두 번 호출된다.
// 다른 사람에게 동기화는 OnRep_BurstCounter 에서 이팩트만 처리하도록 한다.
void APBWeapon::Firing(EWeaponPoint ePoint)
{
	UpdateFirstFiringFlag();

	PreFiring(ePoint);

	bool bHasAmmo = HasAmmoInClip();
	if (bHasAmmo && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			PlayWeaponFireEffect(ePoint);
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			AddBurstEffectOnFire();
			AddDeviationOnFire();
			FireWeapon(ePoint); //
			UseAmmo();

			// update firing FX on remote clients if function was called on server
			NotifyWeaponBurstRecoil(FiringSync.BurstCounter);
			FiringSync.BurstCounter++;
		}
	}
	else if (CurrentAmmoInClip <= 0 && CanReload() && !IsReloading())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// 빈 총의 격발 소리
		if (!HasAmmo())
		{
			PlayWeaponSound(ePoint, EWeaponSoundType::OutofAmmo);
		}

		// stop weapon fire FX, but stay in Firing state
		if (FiringSync.BurstCounter > 0)
		{
			OnFireFinished();
		}
	}

	// Set Refiring, Reload
	PostFiring(ePoint);
}

void APBWeapon::OnFiring()
{	
	bPendingFiring = true;

	if (WeapAttachment)
	{
		WeapAttachment->OnFiring();
	}
}

// Set Refiring , Check Reload
void APBWeapon::PostFiring(EWeaponPoint ePoint)
{	
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerFiring(ePoint);
		}

		bPendingFiring = false;
		bPostFiring = true;

		// setup refire timer 
		bool bDoRefire = bRefiring && (CurrentState == EWeaponState::Firing && WeaponConfig.FireInterval > 0.0f);
		if (bDoRefire && (FiringSync.BurstCounter < DesiredBurstCounter || DesiredBurstCounter < 0))
		{
			PendingFiring(WeaponConfig.FireInterval);
		}
	}

	// Delayed event		
	GetWorldTimerManager().SetTimer(TimerHandle_PostFiringEnd, this, &APBWeapon::PostFiringEnd, WeaponConfig.FireInterval, false);

	LastFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

void APBWeapon::PostFiringEnd()
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// reload after firing last round
		if (!HasAmmoInClip() && CanReload())
		{
			StartReload();
		}
	}

	bPostFiring = false;
}

void APBWeapon::SetReFiring(bool bReFire)
{
	bRefiring = bReFire;
}

bool APBWeapon::ServerFiring_Validate(EWeaponPoint ePoint)
{
	return true;
}

void APBWeapon::ServerFiring_Implementation(EWeaponPoint ePoint)
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	Firing(ePoint);

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		NotifyWeaponBurstRecoil(FiringSync.BurstCounter);
		FiringSync.BurstCounter++;
	}
}

// Firing End
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void APBWeapon::PlayWeaponFireEffect(EWeaponPoint ePoint)
{
	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (WeapAttachment)
	{
		WeapAttachment->PlayWeaponFireEffect(ePoint);
	}
}

void APBWeapon::StopWeaponFireEffects()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	if (WeapAttachment)
	{
		WeapAttachment->StopWeaponFireEffects();
	}
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

void APBWeapon::PlayWeaponSounds(EWeaponSoundType eSound)
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (WeapAttachment)
	{
		WeapAttachment->PlayWeaponSounds(eSound);
	}
}

UAudioComponent* APBWeapon::PlayWeaponSound(EWeaponPoint ePoint, EWeaponSoundType eSound)
{
	if (GetNetMode() == NM_DedicatedServer)
		return nullptr;

	if (WeapAttachment)
	{
		return WeapAttachment->PlayWeaponSound(ePoint, eSound);
	}
	return nullptr;
}

UAudioComponent* APBWeapon::PlayWeaponSoundCue(USoundCue* Sound)
{
	if (GetNetMode() == NM_DedicatedServer)
		return nullptr;

	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UPBGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

float APBWeapon::GetWeaponAnimationLength(EWeaponAnimType eAnim)
{
	if (WeapAttachment)
	{
		return WeapAttachment->GetWeaponAnimationLength(eAnim);
	}

	return 0.f;
}

float APBWeapon::PlayWeaponAnimation(EWeaponAnimType eAnim)
{
	if (WeapAttachment)
	{
		return WeapAttachment->PlayWeaponAnimation(eAnim);
	}

	return 0.f;
}

void APBWeapon::StopWeaponAnimation(EWeaponAnimType eAnim, bool GotoEndSection /*= false*/)
{
	if (WeapAttachment)
	{
		WeapAttachment->StopWeaponAnimation(eAnim, GotoEndSection);
	}
}


float APBWeapon::PlaySpecificWeaponAnim(USkeletalMeshComponent* UseMesh, const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		//weapon animation ( 1st person or 3rd person )
		UAnimMontage* WeapAnim = GetViewerIsFirstPerson() ? Animation.Weap1P : Animation.Weap3P;

		if (WeapAnim)
		{
			Duration = PlayAnimMontage(UseMesh, WeapAnim);
		}

		//character animation ( 1st person or 3rd person )
		UAnimMontage* PawnAnim = GetViewerIsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;

		if (PawnAnim)
		{
			Duration = FMath::Max(Duration, MyPawn->PlayAnimMontage(PawnAnim));
		}
	}

	return Duration;
}

void APBWeapon::StopSpecificWeaponAnim(USkeletalMeshComponent* UseMesh, const FWeaponAnim& Animation, bool GotoEndSection)
{
	if (MyPawn)
	{
		FName StartSectionName = GotoEndSection ? FName(TEXT("End")) : NAME_None;

		//weapon animation ( 1st person or 3rd person )
		UAnimMontage* WeapAnim = GetViewerIsFirstPerson() ? Animation.Weap1P : Animation.Weap3P;
		if (WeapAnim)
		{
			if (StartSectionName != NAME_None)
				MontageJumpToSection(UseMesh, WeapAnim, StartSectionName);
			else
				StopAnimMontage(UseMesh, WeapAnim);
		}

		//character animation ( 1st person or 3rd person )
		UAnimMontage* PawnAnim = GetViewerIsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (PawnAnim)
		{
			if (StartSectionName != NAME_None)
				MyPawn->MontageJumpToSection(PawnAnim, StartSectionName);
			else
				MyPawn->StopAnimMontage(PawnAnim);
		}
	}
}

float APBWeapon::PlayAnimMontage(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	UAnimInstance * AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance)
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}

	return 0.0f;
}

void APBWeapon::StopAnimMontage(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage)
{
	UAnimInstance* AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance && AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		AnimInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime());
	}
}

void APBWeapon::MontageJumpToSection(USkeletalMeshComponent* UseMesh, class UAnimMontage* AnimMontage, FName StartSectionName)
{
	UAnimInstance* AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance && AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		// Start at a given Section.
		if (StartSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
		}
	}
}

FVector APBWeapon::GetCameraAim() const
{
	APBPlayerController* const PlayerController = Instigator ? Cast<APBPlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		auto InstigatorAsCharacter = Cast<APBCharacter>(Instigator);
		if (nullptr != InstigatorAsCharacter)
		{
			FinalAim = InstigatorAsCharacter->GetFirstPersonViewTransform().Rotator().Vector();
		}
		else
		{
			FinalAim = Instigator->GetViewRotation().Vector();
		}
	}

	return FinalAim;
}

FVector APBWeapon::GetAdjustedAim() const
{
	FVector FinalAim = FVector::ZeroVector;
	if (Instigator)
	{
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}
	return FinalAim;
}

FVector APBWeapon::GetStartFireLoc(const FVector& AimDir) const
{
	APBPlayerController* PC = MyPawn ? Cast<APBPlayerController>(MyPawn->Controller) : NULL;
	APBAIController* AIPC = MyPawn ? Cast<APBAIController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}
	else if (AIPC)
	{
		OutStartTrace = GetMuzzleLocation();
	}

	return OutStartTrace;
}

FVector APBWeapon::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh(FiringSync.FirePoint);
	if (UseMesh)
	{
		FName MuzzlePointName = WeapAttachment ? WeapAttachment->GetMuzzlePointName(FiringSync.FirePoint) : FName();
		return UseMesh->GetSocketLocation(MuzzlePointName);
	}	
	return FVector::ZeroVector;
}

FVector APBWeapon::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh(FiringSync.FirePoint);
	if (UseMesh)
	{
		FName MuzzlePointName = WeapAttachment ? WeapAttachment->GetMuzzlePointName(FiringSync.FirePoint) : FName();
		return UseMesh->GetSocketRotation(MuzzlePointName).Vector();
	}
	return FVector::ZeroVector;
}



FHitResult APBWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	FHitResult OutHit(ForceInit);

	if (GetWorld())
	{
		static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

		// Perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(WeaponFireTag, true, GetInstigator());
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		GetWorld()->LineTraceSingleByChannel(OutHit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
	}

	return OutHit;
}

TArray<FHitResult> APBWeapon::WeaponTraces(const FVector& StartTrace, const FVector& EndTrace) const
{
	TArray<FHitResult> OutHits;

	static FName WeaponFireTag = FName(TEXT("WeaponPierceTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, true, GetInstigator());
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceMultiByChannel(OutHits, StartTrace, EndTrace, COLLISION_WEAPON_PIERCE, TraceParams);

	return OutHits;
}

void APBWeapon::AimingTrace(FHitResult& HitResult)
{
	APBPlayerController* const PC = Instigator ? Cast<APBPlayerController>(Instigator->Controller) : NULL;
	if (PC)
	{
		const FVector AimDir = GetAdjustedAim();
		const FVector TraceStart = GetStartFireLoc(AimDir);

		const float BulletRange = 10000.0f;
		const FVector TraceEnd = TraceStart + AimDir * BulletRange;
		HitResult = WeaponTrace(TraceStart, TraceEnd);		
	}
}

const FVector APBWeapon::GetPaniniLocation(const FVector& InLocation, UMaterialInterface* PaniniParamsMat)
{
	if (GetOwnerPawn() && PaniniParamsMat)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(GetOwnerPawn()->GetController());
		ULocalPlayer* LP = PC ? PC->GetLocalPlayer() : nullptr;

		UPBGameViewportClient* Viewport = LP ? Cast<UPBGameViewportClient>(LP->ViewportClient) : nullptr;
		if (Viewport)
		{
			return Viewport->PaniniProjectLocationForPlayer(LP, InLocation, PaniniParamsMat);
		}
	}

	return InLocation;
}

EWeaponAnimType APBWeapon::GetFireAnimType()
{
	return IsZooming() ? EWeaponAnimType::FireZoom : EWeaponAnimType::Fire;
}

void APBWeapon::SetOwningPawn(APBCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);

		if (WeapAttachment)
		{
			WeapAttachment->SetBulletInstigator(NewOwner);
		}
	}	
}
//// melee
bool APBWeapon::ServerMeleeAttackTriggerBegin_Validate()
{
	return true;
}

void APBWeapon::ServerMeleeAttackTriggerBegin_Implementation()
{
	MeleeAttackTriggerBegin();
}

bool APBWeapon::ServerMeleeAttackTriggerEnd_Validate()
{
	return true;
}

void APBWeapon::ServerMeleeAttackTriggerEnd_Implementation()
{
	MeleeAttackTriggerEnd();
}

//// Quick throw
bool APBWeapon::ServerQuickThrowTriggerBegin_Validate(EWeaponType Type)
{
	return true;
}

void APBWeapon::ServerQuickThrowTriggerBegin_Implementation(EWeaponType Type)
{
	QuickThrowTriggerBegin(Type);
}

bool APBWeapon::ServerQuickThrowTriggerEnd_Validate()
{
	return true;
}
void APBWeapon::ServerQuickThrowTriggerEnd_Implementation()
{
	QuickThrowTriggerEnd();
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

void APBWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void APBWeapon::OnRep_BurstCounter()
{
	if (FiringSync.BurstCounter > 0)
	{
		if (WeapAttachment)
		{
			WeapAttachment->PlayWeaponFireEffects();
		}


		// Burst counter increased, it means burst happened with burst counter one minus that.
		NotifyWeaponBurstRecoil(FiringSync.BurstCounter - 1);
	}
	else
	{
		StopWeaponFireEffects();
	}
}

void APBWeapon::OnRep_MeleeAttack()
{
	if (MeleeAttackSync.BurstCounter > 0)
	{
		if (WeapAttachment)
		{
			WeapAttachment->OnMeleeAttack();
		}
	}
	//else
	//{
	//	//looping effect 가 아니므로 주석처리함
	//	if (WeapAttachment)
	//	{
	//		WeapAttachment->OnMeleeAttackEnd();
	//	}
	//}
}

void APBWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		OnStartReload();
	}
	else
	{
		StopReload();
	}
}

void APBWeapon::OnRep_FireReady(TArray<bool> bPreWantsToFireReady)
{
	for (int32 i = 0; i < bWantsToFireReady.Num(); i++)
	{		
		if (bWantsToFireReady[i] != bPreWantsToFireReady[i])
		{
			if (bWantsToFireReady[i])
			{
				PlayWeaponAnimation(GetFireAnimType());
			}
			else
			{
				OnFireStarted(-1);
			}			
		}
	}
}

void APBWeapon::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME(APBWeapon, MyPawn);

	DOREPLIFETIME_CONDITION(APBWeapon, CurrentAmmo,				COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBWeapon, CurrentAmmoInClip,		COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(APBWeapon, FiringSync,				COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APBWeapon, MeleeAttackSync,			COND_SkipOwner);
	DOREPLIFETIME(APBWeapon, bPendingReload);
}

USkeletalMeshComponent* APBWeapon::GetWeaponMesh(EWeaponPoint ePoint) const
{
	return WeapAttachment ? WeapAttachment->GetWeaponMesh(ePoint) : nullptr;
}

USkeletalMeshComponent* APBWeapon::GetSpecificWeapMesh(EWeaponPoint ePoint, bool bFirstPerson) const
{
	return WeapAttachment ? WeapAttachment->GetSpecificWeapMesh(ePoint, bFirstPerson) : nullptr;
}

void APBWeapon::Get1PMeshes(TArray<UMeshComponent*>& Meshes)
{
	if (WeapAttachment)
	{
		for (int32 i = 0; i < WeapAttachment->GetAttachmentDataCount(); i++)
		{
			Meshes.Add(WeapAttachment->GetSpecificWeapMesh(EWeaponPoint(i), true));
		}
	}
}

class APBCharacter* APBWeapon::GetOwnerPawn() const
{
	return MyPawn;
}

bool APBWeapon::GetViewerIsFirstPerson() const
{
	auto PlayerCharacter = Cast<APBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	auto OwnerCharacter = GetOwnerPawn();
	bool bIsViewingOwnself = false;
	bool bIsFirstPerson = false;

	if (nullptr != PlayerCharacter)
	{
		bIsViewingOwnself = PlayerCharacter->IsViewingOwnself();
		if (false == bIsViewingOwnself)
		{
			bIsFirstPerson = PlayerCharacter->IsFirstPersonView();
		}
	}

	if (nullptr != OwnerCharacter && bIsViewingOwnself)
	{
		bIsFirstPerson = OwnerCharacter->IsFirstPersonView();
	}

	return bIsFirstPerson;
}

bool APBWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool APBWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

int APBWeapon::GetBurstCounter() const
{
	return FiringSync.BurstCounter;
}

void APBWeapon::SetFirePoint(EWeaponPoint ePoint)
{
	FiringSync.FirePoint = ePoint;
}

EWeaponPoint APBWeapon::GetFirePoint()
{
	return FiringSync.FirePoint;
}

void APBWeapon::SetFireMode(EFireMode eMode)
{
	FiringSync.FireMode = eMode;
}

EFireMode APBWeapon::GetFireMode()
{
	return FiringSync.FireMode;
}

void APBWeapon::SwitchToBestWeapon()
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		MyPawn->SwitchToBestWeapon();
	}
}

void APBWeapon::FuncTriggerBegin()
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled)
	{
		if (FunctionalTriggerAction)
		{
			FunctionalTriggerAction->BeginState();
		}
	}
}

void APBWeapon::QuickThrowTriggerBegin(EWeaponType Type)
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled && Role < ROLE_Authority)
	{
		ServerQuickThrowTriggerBegin(Type);
	}

	if (QuickThrowTriggerAction)
	{
		auto ThrowAction = Cast<UPBWeapActionQuickThrow>(QuickThrowTriggerAction);
		if (ThrowAction)
		{
			ThrowAction->ThrowingWeapType = Type;
			QuickThrowTriggerAction->BeginState();
		}
		
	}
}

void APBWeapon::QuickThrowTriggerEnd()
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled && Role < ROLE_Authority)
	{
		ServerQuickThrowTriggerEnd();
	}

	if (QuickThrowTriggerAction)
	{
		QuickThrowTriggerAction->EndState();
	}
}

void APBWeapon::MeleeAttackTriggerBegin()
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled && Role < ROLE_Authority)
	{
		ServerMeleeAttackTriggerBegin();
	}

	if (MeleeAttackTriggerAction)
	{
		MeleeAttackTriggerAction->BeginState();
	}
}

void APBWeapon::MeleeAttackTriggerEnd()
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	if (bLocallyControlled && Role < ROLE_Authority)
	{
		ServerMeleeAttackTriggerEnd();
	}

	if (MeleeAttackTriggerAction)
	{
		MeleeAttackTriggerAction->EndState();
	}
}

void APBWeapon::MeleeAttackHitTrace(EAttackDirType AttackDirType)
{	
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		const FVector AimDir = GetAdjustedAim();
		const FVector StartTrace = GetStartFireLoc(AimDir);
		const FVector ShootDir = AimDir;

		APBInst_MeleeBullet* MeleeBullet = GetMeleeBullet();
		if (MeleeBullet)
		{
			FHitResult HitResult;
			if (MeleeBullet->TraceHit(this, StartTrace, ShootDir, AttackDirType, HitResult))
			{
				if (GetNetMode() == NM_Client)
				{
					ServerNotifyMeleeHit(HitResult, StartTrace, ShootDir);
				}
				
				// 서버, 클라 모두 호출 되지만 클라이언트는 hit effect 연출 목적으로 콜한다(Replicated HitNotify 가 SkipOwner 인 상태이므로) 
				OnMeleeAttackHit(HitResult, StartTrace, ShootDir);			
			}
		}
	}
}

void APBWeapon::OnMeleeAttackHit(const FHitResult& HitResult, const FVector& Origin, const FVector& ShootDir)
{
	// deal damage, play effect
	APBInst_MeleeBullet* MeleeBullet = GetMeleeBullet();
	if (MeleeBullet)
	{
		if (GetUseTableTata())
		{
			MeleeBullet->ApplyTableData(GetTableData());
		}

		MeleeBullet->PointHit_Confirm(HitResult, Origin, ShootDir);
	}
}

bool APBWeapon::ServerNotifyMeleeHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void APBWeapon::ServerNotifyMeleeHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal ShootDir)
{
	OnMeleeAttackHit(Impact, Origin, ShootDir);
}

void APBWeapon::OnMeleeAttackStarted()
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		const float GameTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const float AttackInterval = WeaponConfig.MeleeAttackInterval;
		if (LastMeleeAttackTime > 0 && AttackInterval > 0.0f &&	LastMeleeAttackTime + AttackInterval > GameTime)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReMeleeAttack, this, &APBWeapon::OnMeleeAttack, AttackInterval, false);
		}
		else
		{
			OnMeleeAttack();
		}
	}
}

void APBWeapon::OnMeleeAttackFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_ReMeleeAttack);

	MeleeAttackSync.BurstCounter = 0;
}

void APBWeapon::OnMeleeAttack()
{	
	UpdateFirstFiringFlag();	

	if (WeapAttachment)
	{
		WeapAttachment->OnMeleeAttack();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{		
		MeleeAttackSync.BurstCounter++;

		if (Role < ROLE_Authority)
		{
			ServerMeleeAttack();
		}

		// Delayed event		
		const float AttackInterval = WeaponConfig.MeleeAttackInterval;

		GetWorldTimerManager().SetTimer(TimerHandle_PostMeleeAttackEnd, this, &APBWeapon::OnPostMeleeAttackEnd, AttackInterval, false);

		bool bDoRefire = (CurrentState == EWeaponState::MeleeAttack && AttackInterval > 0.0f);

		if (bDoRefire)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReMeleeAttack, this, &APBWeapon::OnReMeleeAttack, AttackInterval, false);
		}

		bPostMeleeAttack = true;
	}

	LastMeleeAttackTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

void APBWeapon::OnReMeleeAttack()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_PostMeleeAttackEnd);

	if (bPostMeleeAttack)
	{
		OnPostMeleeAttackEnd();
	}		

	OnMeleeAttack();
}

void APBWeapon::OnPostMeleeAttackEnd()
{
	bPostMeleeAttack = false;

	//stop animation
	if (WeapAttachment)
	{
		WeapAttachment->OnMeleeAttackEnd();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (!IsMeleeAttacking())
		{
			WakeFireTriggers();
		}			
	}
}

bool APBWeapon::ServerMeleeAttack_Validate()
{
	return true;
}

void APBWeapon::ServerMeleeAttack_Implementation()
{		
	OnMeleeAttack();
	MeleeAttackSync.BurstCounter++;
}

float APBWeapon::SinceFiring()
{
	return GetWorld() ? (GetWorld()->GetTimeSeconds() - LastFireTime) : 0.f;
}

EWeaponState APBWeapon::GetCurrentState() const
{
	return CurrentState;
}

void APBWeapon::SetAmmoForcibly(int32 InCurrentAmmo, int32 InCurrentAmmoInClip)
{
	CurrentAmmo = InCurrentAmmo;
	CurrentAmmoInClip = InCurrentAmmoInClip;
}

int32 APBWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 APBWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 APBWeapon::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

int32 APBWeapon::GetMaxAmmo() const
{
	return WeaponConfig.MaxAmmo;
}

FZoomInfo APBWeapon::GetZoomInfo() const
{
	return WeaponConfig.ZoomInfo;
}

void APBWeapon::SetZooming(bool bZoom)
{
	if (bZoom)
	{
		StopWeaponAnimation(EWeaponAnimType::Fire);
		PlayWeaponSoundCue(WeaponConfig.ZoomInfo.ZoomInSound);
	}
	else
	{
		StopWeaponAnimation(EWeaponAnimType::FireZoom);
		PlayWeaponSoundCue(WeaponConfig.ZoomInfo.ZoomOutSound);
	}
}

bool APBWeapon::IsZooming() const
{
	return MyPawn ? MyPawn->IsZooming() : false;
}

bool APBWeapon::IsReloading() const
{
	return (bool)bPendingReload || CurrentState == EWeaponState::Reloading;
}

bool APBWeapon::IsEquiping() const
{
	return (bool)bPendingEquip || CurrentState == EWeaponState::Equipping;
}

bool APBWeapon::IsFiring() const
{
	return  GetBurstCounter() > 0 || bPendingFiring || bPostFiring;
}

bool APBWeapon::IsMeleeAttacking() const
{
	return (bPostMeleeAttack || MeleeAttackSync.BurstCounter > 0);
}

bool APBWeapon::HasInfiniteAmmo() const
{
	const APBPlayerController* MyPC = (MyPawn != NULL) ? Cast<const APBPlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteAmmo || (MyPC && MyPC->HasInfiniteAmmo());
}

void APBWeapon::SetInfiniteAmmo(bool bSet)
{
	WeaponConfig.bInfiniteAmmo = bSet;
}

bool APBWeapon::HasInfiniteClip() const
{
	const APBPlayerController* MyPC = (MyPawn != NULL) ? Cast<const APBPlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteClip || (MyPC && MyPC->HasInfiniteClip());
}

void APBWeapon::SetInfiniteClip(bool bSet)
{
	WeaponConfig.bInfiniteClip = bSet;
}

float APBWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float APBWeapon::GetEquipDuration() const
{
	if (WeaponConfig.EquipDuration > 0)
	{
		// We have cached value
		return WeaponConfig.EquipDuration;
	}
	else if (WeapAttachment)
	{
		return WeapAttachment->GetWeaponAnimationLength(EWeaponAnimType::Equip);
	}

	return 0.f;
}

EWeaponStance APBWeapon::GetWeaponStance() const
{
	return WeaponStance;
}

const FWeaponBasePose& APBWeapon::GetWeaponPose()
{
	auto GI = Cast<UPBGameInstance>(UGameplayStatics::GetGameInstance(this));
	const bool bLobbyAnim = GI ? !GI->IsCurrentState(PBGameInstanceState::Playing) : false;

	if (bLobbyAnim && MyPawn)
	{
		switch (MyPawn->GetCharaterData().Gender)
		{
		case EPBCharacterGender::Male:		return WeaponPose_LobbyMale.HasData() ? WeaponPose_LobbyMale : WeaponPose;
		case EPBCharacterGender::Female:	return WeaponPose_LobbyFemale.HasData() ? WeaponPose_LobbyFemale : WeaponPose;
		}	
	}

	return WeaponPose;
}

float APBWeapon::GetRecoilReturnRate() const
{
	return UPBGameplayStatics::InterpDelayToInterpSpeed((GetCurrentState() == EWeaponState::Firing) ? WeaponRecoilConfig.RecoilReturnDelay_Firing : WeaponRecoilConfig.RecoilReturnDelay_Normal);
}

float APBWeapon::GetCamRecoilRatio() const
{
	return WeaponRecoilConfig.CamRecoilRatio;
}

float APBWeapon::GetCamSyncRate() const
{
	return UPBGameplayStatics::InterpDelayToInterpSpeed(WeaponRecoilConfig.CamSyncDelay);
}

bool APBWeapon::GetCrossHairPositionFixed() const
{
	return WeaponCrossHairConfig.CrossHairPositionFixed;
}

TSubclassOf<UPBCrossHairImageWidget> APBWeapon::GetCrossHairImageWidgetClass() const
{
	return WeaponCrossHairConfig.CrossHairImageWidget;
}

bool APBWeapon::GetZoomCrossHairPositionFixed() const
{
	return WeaponCrossHairConfig.ZoomCrossHairPositionFixed;
}

TSubclassOf<UPBCrossHairImageWidget> APBWeapon::GetZoomCrossHairImageWidgetClass() const
{
	return WeaponCrossHairConfig.ZoomCrossHairImageWidget;
}

float APBWeapon::GetZoomEyeSpeedMultiplier() const
{
	return WeaponEyeSpeedConfig.ZoomEyeSpeedMultiplier;
}

float APBWeapon::GetFireEyeSpeedMultiplier() const
{
	return WeaponEyeSpeedConfig.FireEyeSpeedMultiplier;
}


bool APBWeapon::IsGun(EWeaponType type)
{
	switch (type)
	{
	case EWeaponType::AR:
	case EWeaponType::SMG:
	case EWeaponType::SR:
	case EWeaponType::SG:
	case EWeaponType::PS:
		return true;

	}

	return false;
}

bool APBWeapon::IsExplosive(EWeaponType type)
{
	switch (type)
	{
	case EWeaponType::BOMB:
	case EWeaponType::GRENADE:
	case EWeaponType::LC:
	case EWeaponType::RPG:
		return true;

	}

	return false;
}

float APBWeapon::GetPiercingPower() const
{
	return WeaponConfig.PiercingPower;
}
