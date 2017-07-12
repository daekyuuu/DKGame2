// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effects/PBImpactEffect.h"
#include "Weapons/PBWeaponData.h"

// Sets default values
APBProjectile::APBProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;	
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Overlap);
	RootComponent = CollisionComp;

	ProjMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ProjMesh"));
	ProjMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	ProjMesh->bReceivesDecals = false;
	ProjMesh->CastShadow = false;
	ProjMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ProjMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjMesh->SetupAttachment(RootComponent);

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->bAutoDestroy = true;
	ParticleComp->SetupAttachment(RootComponent);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bShouldBounce = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
	CurrentBounceNum = 0;
	MinBounceNum = 0;
	bWantsDestroy = false;
	FailsafeForceDestroyTimer = 10.0f;

	bShowDebugLine = false;

	bStopped = false;
	bInitializedLifeSpan = false;
}

void APBProjectile::LifeSpanExpired()
{
	if (bWantsDestroy)
	{
		Destroy();
		return;
	}

	WantsDestroy();
}

void APBProjectile::OnRep_Instigator()
{
	Super::OnRep_Instigator();

	if (CollisionComp)
	{
		CollisionComp->MoveIgnoreActors.Add(Instigator);
		if (Instigator)
		{
			Instigator->MoveIgnoreActorAdd(this);
		}
	}

	if (Role == ROLE_Authority)
	{
		InstigatorController = GetInstigatorController();
	}
}

void APBProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MovementComp->bShouldBounce)
	{
		MovementComp->OnProjectileBounce.AddDynamic(this, &APBProjectile::OnBounce);
		MovementComp->OnProjectileStop.AddDynamic(this, &APBProjectile::OnStop);
	}
	else
	{
		MovementComp->OnProjectileStop.AddDynamic(this, &APBProjectile::OnPointHit);
		bStopped = true;
	}

	CurrentBounceNum = 0;

	InitialLifeSpan();
}

void APBProjectile::InitialLifeSpan()
{
	SetLifeSpan(ProjData.ProjectileLife);

	bInitializedLifeSpan = true;

	const bool bTimerExists = GetWorldTimerManager().TimerExists(TimerHandle_LifeSpanExpired);
	if (!bTimerExists && FailsafeForceDestroyTimer >= 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_ForceDestroy, this, &APBProjectile::FailSafeForceDestroy, FailsafeForceDestroyTimer, false);
	}
}

// Called every frame
void APBProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool APBProjectile::ApplyTableData(const struct FPBWeaponTableData* Data)
{
	if (Data)
	{
		ProjData.ExplosionDamage = Data->ExplosionDamage;
		ProjData.ExplosionRadius = Data->ExplosionRadius;
	}

	return true;
}

void APBProjectile::OnBounce(const FHitResult& HitResult, const FVector& OldVelocity)
{
	OnPointHit(HitResult);
}

void APBProjectile::OnPointHit(const FHitResult & HitResult)
{
	SavedHitResult = HitResult;
	CurrentBounceNum++;

	ApplyImpulse(HitResult);
	SpawnImpactEffects(HitResult);
}

void APBProjectile::OnStop(const FHitResult& HitResult)
{
	bStopped = true;
}

float APBProjectile::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// 그냥 참고: bCanBeDamaged 가 true 이면 이 함수가 콜 되지 않음.

	if (IsPendingDestroy() || IsPendingKill())
	{
		return 0.f;
	}		

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void APBProjectile::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{

}

void APBProjectile::ReportKills(AController* Killer, const TArray<FTakeDamageInfo>& TakeDamageInfos, FDamageEvent const& DamageEvent)
{
	if (Role != ROLE_Authority)
	{
		return;
	}

	TArray<AController*> KilledControllers;
	
	for (auto& info : TakeDamageInfos)
	{
		APBCharacter* PBCharacter = Cast<APBCharacter>(info.Victim);

		if (PBCharacter && PBCharacter->GetController() && !PBCharacter->IsAlive() && info.ActualDamage > 0.0f)
		{
			// add all killed controllers.
			KilledControllers.Add(PBCharacter->GetController());
		}
	}

	if (KilledControllers.Num() > 0)
	{
		auto GM = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
		if (GM)
		{
			GM->UpdateKillMark(Killer, KilledControllers, this, TakeDamageInfos);
		}
	}
}

void APBProjectile::WantsDestroy(bool bHideImmediately/* = true*/, float PendingDestroy/* = 0.5f*/)
{
	if (IsPendingDestroy())
		return;

	Stop();
	
	if (bHideImmediately)
	{
		GetRootComponent()->SetVisibility(false, true);
	}

	// 다른 플레이어와의 동기화를 위해 바로 삭제하지 않고 약간 딜레이를 준다.(최소 0.5f)
	const float MinimumPendingDestroy = FMath::Max(PendingDestroy, 0.5f);
	SetLifeSpan(MinimumPendingDestroy);

	// 다음 타이머 종료시에 완전 삭제 처리 되고 더 이상 로직을 타지 않게 함
	bWantsDestroy = true;

	if (FailsafeForceDestroyTimer >= 0.f)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ForceDestroy);
	}	
}

void APBProjectile::FailSafeForceDestroy()
{
	WantsDestroy();
}

void APBProjectile::InitVelocity(FVector& ShootDirection)
{
	if (MovementComp)
	{
		MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
	}
}

bool APBProjectile::IsPendingDestroy()
{
	return bWantsDestroy;
}

void APBProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComp)
	{
		MovementComp->Velocity = NewVelocity;
	}
}

void APBProjectile::ApplyImpulse(const FHitResult& HitResult)
{
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();

	if (HitComponent)
	{
		bool bIsSimulatingPhysics = HitComponent->IsSimulatingPhysics();
		if (bIsSimulatingPhysics)
		{
			FVector HitImpulse = GetVelocity() * ProjData.PointHitPhysicsImpulse;
			FVector HitLocation = HitResult.ImpactPoint;
			HitComponent->AddImpulseAtLocation(HitImpulse, HitLocation);
		}
	}
}

void APBProjectile::SpawnImpactEffects(const FHitResult& HitResult)
{
	if (GetWorld() == nullptr)
	{
		return;
	}

	if (ImpactTemplate && HitResult.bBlockingHit)
	{
		FTransform const SpawnTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint);
		APBImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<APBImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = HitResult;
			EffectActor->Instigator = Instigator;
			EffectActor->SetOwner(this);
			UPBGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}
}

void APBProjectile::Stop()
{
	if (MovementComp)
		MovementComp->StopMovementImmediately();
}

const struct FProjectileData& APBProjectile::GetProjectileData() const
{
	return ProjData;
}

