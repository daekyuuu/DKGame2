// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBProj_Explosive.h"
#include "Effects/PBExplosionEffect.h"
#include "PBGameplayStatics.h"

APBProj_Explosive::APBProj_Explosive()
{
	bExploded = false;
	MovementComp->bShouldBounce = true;
	ExpirationType = EProjExpirationType::OnlyTimeout;
}

void APBProj_Explosive::LifeSpanExpired()
{	
	SetLifeSpan(-1.0f);

	if (bWantsDestroy)
	{
		Destroy();
		return;
	}

	DetermineExpired();
}

void APBProj_Explosive::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APBProj_Explosive::InitialLifeSpan()
{
	bool bForceDestroy = true;

	switch (ExpirationType)
	{
	case EProjExpirationType::ProgrammersForce:
		bForceDestroy = false;
		break;
	case EProjExpirationType::OnlyTimeout:
	case EProjExpirationType::BounceAndTimeout:
	case EProjExpirationType::BounceOrTimeout:
	//case EProjExpirationType::BounceToTimeout:	//Bounce 이후에 Time 설정함
	//case EProjExpirationType::StopToTimeout:		//Stop 이후에 Time 설정함
		SetLifeSpan(ProjData.ProjectileLife);
		bForceDestroy = false;
		bInitializedLifeSpan = true;
		break;
	}

	const bool bTimerExists = GetWorldTimerManager().TimerExists(TimerHandle_LifeSpanExpired);
	if ((!bTimerExists || bForceDestroy) && FailsafeForceDestroyTimer >= 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_ForceDestroy, this, &APBProjectile::FailSafeForceDestroy, FailsafeForceDestroyTimer, false);
	}
}

void APBProj_Explosive::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBProj_Explosive, bExploded);
}

void APBProj_Explosive::OnBounce(const FHitResult& HitResult, const FVector& OldVelocity)
{
	OnPointHit(HitResult);
}

void APBProj_Explosive::OnPointHit(const FHitResult& HitResult)
{
	Super::OnPointHit(HitResult);
	
	DetermineExpired();
}


void APBProj_Explosive::OnStop(const FHitResult& HitResult)
{
	Super::OnStop(HitResult);

	DetermineExpired();
}

//Self damaged(총에 맞으면 터지게 한다든가..)
float APBProj_Explosive::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		DetermineExpired(true);
	}		

	return ActualDamage;
}

void APBProj_Explosive::DetermineExpired(bool bForce)
{
	bool bExpired = false;
	const float LifeSpan = GetWorldTimerManager().GetTimerRemaining(TimerHandle_LifeSpanExpired);
	bool bTimeOut = (bInitializedLifeSpan && LifeSpan <= 0.f);

	switch (ExpirationType)
	{
	case EProjExpirationType::OnlyTimeout:
		bExpired = bTimeOut;
		break;
	case EProjExpirationType::BounceToTimeout:
		{
			if (CurrentBounceNum == MinBounceNum)
			{
				SetLifeSpan(ProjData.ProjectileLife);
				bInitializedLifeSpan = true;
				bTimeOut = false;
			}

			bExpired = (bTimeOut && CurrentBounceNum >= MinBounceNum);
		}
		break;
	case EProjExpirationType::BounceAndTimeout:
		bExpired = (bTimeOut && CurrentBounceNum >= MinBounceNum);
		break;
	case EProjExpirationType::BounceOrTimeout:
		bExpired = (bTimeOut || CurrentBounceNum >= MinBounceNum);
		break;
	case EProjExpirationType::OnlyBounce:
		bExpired = (CurrentBounceNum >= MinBounceNum);
		break;
	case EProjExpirationType::OnlyFirstBounce:
		bExpired = (CurrentBounceNum >= 1);
		break;
	case EProjExpirationType::OnlyStop:
		bExpired = bStopped;
		break;
	case EProjExpirationType::StopToTimeout:
		{
			if (bStopped && !bInitializedLifeSpan)
			{
				SetLifeSpan(ProjData.ProjectileLife);
				bInitializedLifeSpan = true;
				bTimeOut = false;
			}

			bExpired = (bStopped && bTimeOut);
		}
		
		break;
	}

	if (bExpired || bForce)
	{
		Expired();
	}
}

void APBProj_Explosive::Expired()
{
	const bool bHideImmediately = (ProjData.ExplosionDuration <= 0.f);

	WantsDestroy(bHideImmediately, ProjData.ExplosionDuration);

	if (Role == ROLE_Authority && !bExploded)
	{
		Explode(GetSpecificHitResult());
	}
}

void APBProj_Explosive::Explode(const FHitResult& Impact)
{
	AGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AGameMode>() : nullptr;
	if (GM && !(GM->GetMatchState() == MatchState::InProgress))
	{
		// Do not explode when the game is not running
		return;
	}

	if (ParticleComp)
	{
		ParticleComp->Deactivate();
	}
	
	// debug
	if (ProjData.ExplosionRadius > 0 && bShowDebugLine)
	{
		UPBGameplayStatics::ShowDebugSphere(this, GetActorLocation(), ProjData.ExplosionRadius, FColor::Red, 30.f);
	}	

	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	if (ProjData.ExplosionDamage > 0 && ProjData.ExplosionRadius > 0 && ProjData.DamageType)
	{
		FRadialDamageEvent EmptyDmgEvent;
		EmptyDmgEvent.DamageTypeClass = ProjData.DamageType;

		TArray<FTakeDamageInfo> OutDamagedInfos;
	
		UPBGameplayStatics::ApplyRadialDamage(this, ProjData.ExplosionDamage, NudgedImpactLocation, ProjData.ExplosionRadius, ProjData.DamageType, 
			TArray<AActor*>(), OutDamagedInfos, this, InstigatorController.Get(), false, COLLISION_PROJECTILE);

		// set weapon id to TakeDamageInfo
		for (auto& Info : OutDamagedInfos)
		{
			Info.ItemId = WeaponId;
		}
		ReportKills(InstigatorController.Get(), OutDamagedInfos, EmptyDmgEvent);
	}

	if (ExplosionTemplate)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		APBExplosionEffect* const EffectActor = GetWorld() ? GetWorld()->SpawnActorDeferred<APBExplosionEffect>(ExplosionTemplate, SpawnTransform) : nullptr;
		if (EffectActor)
		{
			EffectActor->SurfaceHit = Impact;
			EffectActor->Instigator = Instigator;
			EffectActor->SetOwner(this);
			UPBGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	bExploded = true;
}

void APBProj_Explosive::OnRep_Exploded()
{
	Explode(GetSpecificHitResult());
}

FHitResult& APBProj_Explosive::GetSpecificHitResult()
{
	if (GetWorld())
	{
		FVector ProjDirection = GetActorForwardVector();

		const FVector StartTrace = GetActorLocation();
		const FVector EndTrace = GetActorLocation() + FVector(0.f, 0.f, -150);
		//FHitResult Impact;
		static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

		// Perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(WeaponFireTag, true, GetInstigator());
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;

		if (!GetWorld()->LineTraceSingleByChannel(SavedHitResult, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams))//FCollisionQueryParams(TEXT("ProjClient"), true, Instigator)))
		{
			// failsafe
			SavedHitResult.ImpactPoint = GetActorLocation();
			SavedHitResult.ImpactNormal = -ProjDirection;
		}
	}

	return SavedHitResult;
}

void APBProj_Explosive::ForceExplode()
{
	FHitResult HitResult;
	HitResult.ImpactPoint = GetActorLocation();
	HitResult.ImpactNormal = -GetActorForwardVector();

	Explode(HitResult);
	WantsDestroy();
}
