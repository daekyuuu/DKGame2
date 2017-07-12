// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_Throw.h"


APBWeap_Throw::APBWeap_Throw()
{
	WeaponType = EWeaponType::GRENADE;
	FireStartOffsetDistance = 0.f;
	FireStartOffsetHeight = 0.f;
	DistanceToThrowSpeedRatio = nullptr;
	FireEmitDelay = 0.1f;
	bQuickThrowing = false;
}

void APBWeap_Throw::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APBWeap_Throw, bWantsToFireReady, COND_SkipOwner);
}

bool APBWeap_Throw::CanEquip() const
{
	return HasAmmo();
}

bool APBWeap_Throw::CanFire() const
{
	if (!Super::CanFire())
		return false;

	return HasAmmo();
}

void APBWeap_Throw::OnEquip(const APBWeapon* LastWeapon)
{
	Super::OnEquip(LastWeapon);
	bQuickThrowing = false;
}

void APBWeap_Throw::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;

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

	DetermineWeaponStates();
}

void APBWeap_Throw::StartFire(EFireTrigger eFireTrigger, int32 BurstCount /*= -1*/)
{
	if (CanFire() && !IsInFireInterval())
	{			
		FireReady(eFireTrigger);
	}
}

//Throw Start, Loop Anim
void APBWeap_Throw::FireReady(EFireTrigger eFireTrigger)
{	
	bPendingFiring = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_Emit);

	SetWeaponFireReadyState(eFireTrigger, true);

	PlayWeaponAnimation(GetFireAnimType());
}

static EFireTrigger EndFireTrigger;

void APBWeap_Throw::EndFire(EFireTrigger eFireTrigger)
{
	SetWeaponFireReadyState(eFireTrigger, false);
	SetWeaponFireState(eFireTrigger, true);
	EndFireTrigger = eFireTrigger;	
}

void APBWeap_Throw::OnFireStarted(int32 BurstCount)
{
	const bool bLocallyControlled = MyPawn && MyPawn->IsLocallyControlled();

	bool ThrowingAnim = true;
	StopWeaponAnimation(GetFireAnimType(), ThrowingAnim);

	//수류탄 놓는 소리
	if (GetNetMode() != NM_DedicatedServer)
	{
		PlayWeaponSound(FiringSync.FirePoint, EWeaponSoundType::Fire);
		PlayFireForceFeedback(FiringSync.FirePoint);
		PlayFireCameraShake(FiringSync.FirePoint);
	}

	//Voice : Fire in the hole~~
	if (MyPawn)
	{
		MyPawn->PlayVoice(EPBVoiceType::ThrowGrenade);
	}

	//Emit
	if (bLocallyControlled)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Emit, this, &APBWeap_Throw::OnFiring, FireEmitDelay, false);
	}

	LastFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

//Emit
void APBWeap_Throw::Firing(EWeaponPoint ePoint)
{
	UpdateFirstFiringFlag();

	PreFiring(ePoint);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		FireWeapon(ePoint);
		UseAmmo();
	}

	PostFiring(ePoint);
}

void APBWeap_Throw::PostFiring(EWeaponPoint ePoint)
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerFiring(ePoint);
		}		
	}

	// WeaponConfig.FireInterval 은 발사 동작이(EndFire) 시작하는 시점 부터이므로(Emit 시점이 아님) 보정함
	float AdjustedFireInterval = FMath::Clamp(WeaponConfig.FireInterval - FireEmitDelay, 0.1f, WeaponConfig.FireInterval);
	GetWorldTimerManager().SetTimer(TimerHandle_PostFiringEnd, this, &APBWeap_Throw::PostFiringEnd, AdjustedFireInterval, false);

	bPendingFiring = false;
	bPostFiring = true;
	//UpdateMeshVisibilitys(true); //애니메이션에서 처리하자
}

void APBWeap_Throw::PostFiringEnd()
{
	//Super::PostFiringEnd(); (재장전 로직이 들어 있으므로 호출하지 말것)

	OnFireFinished();
	SetWeaponFireState(EndFireTrigger, false);
	SetWeaponState(EWeaponState::Idle);

	if (MyPawn)
	{
		if (bQuickThrowing)
		{
			MyPawn->OnQuickThrowEnd();
			return;
		}

		// reload after firing last round 
		if (MyPawn->IsLocallyControlled())
		{
			if (!HasAmmoInClip())
			{
				if (CanReload())
				{
					StartReload();
				}
				else
				{
					if (MyPawn->IsLocallyControlled())
					{
						MyPawn->SwitchToBestWeapon();
					}
				}
			}
			else
			{
				PlayWeaponSounds(EWeaponSoundType::Equip);
				//UpdateMeshVisibilitys(true); //애니메이션에서 처리하자
			}
		}	
	}
}

void APBWeap_Throw::StopReload()
{
	Super::StopReload();

	PlayWeaponSounds(EWeaponSoundType::Equip);
	//UpdateMeshVisibilitys(true); //애니메이션에서 처리하자
}

void APBWeap_Throw::SetQuickThrowing()
{
	bQuickThrowing = true;
}

void APBWeap_Throw::FireWeapon(EWeaponPoint ePoint)
{
	FVector ShootDir = GetAdjustedAim();
	FVector ShootOrigin = GetStartFireLoc(ShootDir);
	const float ProjectileAdjustRange = 10000.0f;
	const FVector EndTrace = ShootOrigin + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(ShootOrigin, EndTrace);

	float DistanceToTarget = Impact.bBlockingHit ? Impact.Distance : ProjectileAdjustRange;
	float ThrowSpeedRatio = DistanceToThrowSpeedRatio ? DistanceToThrowSpeedRatio->GetFloatValue(DistanceToTarget) : 0.f;
	ThrowSpeedRatio = FMath::Max(ThrowSpeedRatio, 0.f);

	//offset
	ShootOrigin += ShootDir * FireStartOffsetDistance;
	ShootDir += FVector(0.f, 0.f, FireStartOffsetHeight);

	ServerFireProjectile(ePoint, ShootOrigin, ShootDir, ThrowSpeedRatio);
}
