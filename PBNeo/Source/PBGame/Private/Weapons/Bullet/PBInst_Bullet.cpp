// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBInst_Bullet.h"
#include "Effects/PBImpactEffect.h"
#include "Weapons/PBWeapon.h"
#include "Weapons/PBWeap_Instant.h"
#include "Weapons/PBWeapLineTrace.h"
#include "Player/PBCharacter.h"
#include "Mode/PBGameMode.h"
#include "Particles/ParticleSystemComponent.h"
#include "PBCheatManager.h"
#include "PBGameUserSettings.h"
#include "Table/Game/PBGameTableManager.h"
#include "Table/Game/PBMaterialTable.h"


// Sets default values
APBInst_Bullet::APBInst_Bullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BulletFX.ImpactTemplate = APBImpactEffect::StaticClass();
	HitTrace = CreateDefaultSubobject<UPBWeapLineTrace>(TEXT("WeaponTrace"));
}

// Called when the game starts or when spawned
void APBInst_Bullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APBInst_Bullet::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}
void APBInst_Bullet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APBInst_Bullet, HitNotify, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APBInst_Bullet, PierceHitNotify, COND_SkipOwner);
}

bool APBInst_Bullet::ApplyTableData(const struct FPBWeaponTableData* Data)
{
	bool bIsDirty = false;

	if (Data)
	{
		bIsDirty = BulletData.ImportTableData(Data, bIsDirty);
	}

	return bIsDirty;
}

void APBInst_Bullet::PointHit_Confirm(const FHitResult& Impact, const FVector& ShootPoint, const FVector& ShootDir)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = ShootPoint;
		HitNotify.ShootDir = ShootDir;
	}

	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		// 동일 팀 캐릭터 이펙트 재생 체크
		APBCharacter* OwnerCharacter = Cast<APBCharacter>(Instigator);
		APBCharacter* TargetCharacter = Cast<APBCharacter>(Impact.GetActor());

		if (TargetCharacter != NULL && OwnerCharacter != NULL)
		{
			if (OwnerCharacter->GetTeamType() != TargetCharacter->GetTeamType())
			{
				SpawnImpactEffects(Impact);
			}
		}
		else
		{
			SpawnImpactEffects(Impact);
		}

		const FVector EndTrace = ShootPoint + ShootDir * BulletData.BulletRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		APBWeapon* Weapon = Cast<APBWeapon>(GetOwner());
		if (Weapon)
		{
			const FVector& MuzzlePoint = Weapon->GetMuzzleLocation();
			SpawnTrailEffect(MuzzlePoint, EndPoint);
		}		
	}
}

float APBInst_Bullet::PointHit_Confirm(const FPierceHitResult& PierceImpact, const FVector& ShootPoint, const FVector& ShootDir, float PiercingPower)
{
	// 대미지 감소율 계산
	float FinalPiercingPower = PiercingPower;

	// handle damage
	if (ShouldDealDamage(PierceImpact.Impact.GetActor()))
	{
		DealDamage(PierceImpact, ShootDir, FinalPiercingPower);
	}

	// 재질 테이블 데이터 Get
	UPhysicalMaterial* HitPhysMat = PierceImpact.Impact.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);
	auto MaterialTable = UPBGameplayStatics::GetGameTableManager(this)->GetMaterialTableData(HitSurfaceType);

	float MaterialPiercingReduceRate = MaterialTable->PiercingReduceRate;	// 오브젝트 재질 관통 방어율
	float PiercingThickness = 0.0f;											// 오브젝트 관통 두께 (cm)

	// 관통 Actor 정보가 있을 경우만 관통 두께 계산
	APBCharacter* Character = Cast<APBCharacter>(PierceImpact.ImpactPierce.GetActor());
	if (PierceImpact.ImpactPierce.GetActor() && Character == NULL)
	{
		float Dist = FVector::Dist(PierceImpact.ImpactPierce.ImpactPoint, PierceImpact.Impact.ImpactPoint);
		PiercingThickness = Dist;
	}
	else if(PierceImpact.ImpactPierce.GetActor() && Character != NULL)
	{
		PiercingThickness = 1.0f;
	}

	float PiercingReduce = MaterialPiercingReduceRate * PiercingThickness;	// 관통 감소율 (재질 감소율 * 두께(cm) )
	FinalPiercingPower = PiercingPower - PiercingReduce;					// 최종 감소율 (최종 관통력)

	// Pierce Hit Info Log 표시
	if (UPBCheatManager::DebugPierceToggle)
	{
		if (PiercingPower > 0.0f || PierceImpact.IsFirstHit)
		{
			float Damage = 0.0f;

			if (PierceImpact.IsFirstHit)
			{
				Damage = BulletData.BulletHitDamage;
			}
			else
			{
				Damage = BulletData.BulletHitDamage * FMath::Clamp(PiercingPower, 0.0f, 1.0f);
			}


			GEngine->AddOnScreenDebugMessage(-1,
				10.f,
				FColor::White,
				FString::Printf(TEXT("TargetName : %s / Thickness : %.1f cm / Reduce : %.1f / PiercingPower : %.1f / BulletDamage : %.1f"), *(PierceImpact.Impact.GetActor()->GetName()), PiercingThickness, PiercingReduce, PiercingPower, Damage)
			);
		}
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		// 정면부 이펙트 처리
		// 관통되기 전 관통력이 0보다 크며, 첫 히트 오브젝트일 경우 이펙트 재생
		if (PiercingPower > 0.0f || PierceImpact.IsFirstHit)
		{
			SpawnImpactEffects(PierceImpact.Impact);
		}

		// 관통된 후면부 이펙트 처리
		// 최종 관통력이 0 이하이면 후면부 이펙트를 표시하지 않는다.
		if (PierceImpact.ImpactPierce.GetActor() && FinalPiercingPower > 0.0f)
		{
			SpawnImpactEffects(PierceImpact.ImpactPierce);
		}

		const FVector EndTrace = ShootPoint + ShootDir * BulletData.BulletRange;
		const FVector EndPoint = PierceImpact.Impact.GetActor() ? PierceImpact.Impact.ImpactPoint : EndTrace;

		APBWeapon* Weapon = Cast<APBWeapon>(GetOwner());
		if (Weapon)
		{
			const FVector& MuzzlePoint = Weapon->GetMuzzleLocation();
			SpawnTrailEffect(MuzzlePoint, EndPoint);
		}
	}

	return FinalPiercingPower;
}

void APBInst_Bullet::PointHit_Miss(const FVector& ShootPoint, const FVector& ShootDir)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = ShootPoint;
		HitNotify.ShootDir = ShootDir;
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = ShootPoint + ShootDir * BulletData.BulletRange;
		SpawnTrailEffect(ShootPoint, EndTrace);
	}
}

void APBInst_Bullet::OnRep_HitNotify()
{
	const FVector StartTrace = HitNotify.Origin;
	const FVector EndTrace = StartTrace + HitNotify.ShootDir * BulletData.BulletRange;

	FHitResult Impact;
	if (HitTrace)
	{
		HitTrace->LineTraceHit(Cast<APBWeapon>(GetOwner()), StartTrace, EndTrace, Impact);
	}

	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);

		const FVector& EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
		SpawnTrailEffect(StartTrace, EndPoint);
	}
	else
	{
		SpawnTrailEffect(StartTrace, EndTrace);
	}
}

void APBInst_Bullet::OnRep_PierceHitNotify()
{
	const FVector StartTrace = PierceHitNotify.Origin;
	const FVector EndTrace = StartTrace + PierceHitNotify.ShootDir * BulletData.BulletRange;
	const TArray<FPierceHitResult> PierceHitResult = PierceHitNotify.PierceHitResult;

	if (PierceHitResult.Num() > 0)
	{
		int32 LastIndex = PierceHitResult.Num() - 1;
		
		for (int32 i = 0; i < PierceHitResult.Num(); ++i)
		{
			// 정면
			SpawnImpactEffects(PierceHitResult[i].Impact);

			// 후면
			if (PierceHitResult[i].ImpactPierce.GetActor() != NULL && PierceHitResult[i].ResultPiercingPower > 0.0f)
			{
				SpawnImpactEffects(PierceHitResult[i].ImpactPierce);
			}

			if (i == LastIndex)
			{
				const FVector& EndPoint = PierceHitResult[i].ImpactPierce.GetActor() ? PierceHitResult[i].ImpactPierce.ImpactPoint : EndTrace;
				SpawnTrailEffect(StartTrace, EndPoint);
			}

			if (PierceHitResult[i].ResultPiercingPower <= 0.0f)
			{
				break;
			}
		}
	}
	else
	{
		SpawnTrailEffect(StartTrace, EndTrace);
	}
}

void APBInst_Bullet::SpawnImpactEffects(const FHitResult& Impact)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}		

	if (BulletFX.ImpactTemplate && Impact.GetActor())
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			if (HitTrace)
			{
				const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
				const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
				HitTrace->LineTraceHit(Cast<APBWeapon>(GetOwner()), StartTrace, EndTrace, UseImpact);
			}
		}

		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		APBImpactEffect* EffectActor = GetWorld() ? GetWorld()->SpawnActorDeferred<APBImpactEffect>(BulletFX.ImpactTemplate, SpawnTransform) : nullptr;
		if (EffectActor)
		{
			EffectActor->SurfaceHit = Impact;
			EffectActor->Instigator = GetInstigator();
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}
}

void APBInst_Bullet::SpawnTrailEffect(const FVector& StartPoint, const FVector& EndPoint)
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	//const bool bFirstPerson = GetOwnerPawn() ? GetOwnerPawn()->IsFirstPersonView() : false;
	const bool bFirstPerson = GetViewerIsFirstPerson();
	FVector AdjustedStartPoint = StartPoint;

	// apply panini projection position	
	if (bFirstPerson)
	{		
		APBWeapon* Weapon = Cast<APBWeapon>(GetOwner());				
		if (Weapon)
		{
			USkeletalMeshComponent* Mesh1P = Weapon->GetSpecificWeapMesh(BulletPoint, bFirstPerson);
			if (Mesh1P)
			{
				AdjustedStartPoint = Weapon->GetPaniniLocation(StartPoint, Mesh1P->GetMaterial(0));
			}			
		}
	}

	UParticleSystem* TrailFX = bFirstPerson ? BulletFX.TrailFX_1P : BulletFX.TrailFX_3P;
	if (TrailFX)
	{
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, AdjustedStartPoint);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(BulletFX.TrailParamName, EndPoint);
		}
	}
}

bool APBInst_Bullet::TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, EAttackDirType AttackDirType, FHitResult& OutHit)
{
	if (HitTrace)
	{
		return HitTrace->TraceHit(Weapon, StartTrace, ShootDir, BulletData.BulletRange, AttackDirType, OutHit);
	}
	return false;
}

bool APBInst_Bullet::TraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, EAttackDirType AttackDirType, TArray<FHitResult>& OutHits)
{
	if (HitTrace)
	{
		return HitTrace->TraceHits(Weapon, StartTrace, ShootDir, BulletData.BulletRange, AttackDirType, OutHits);
	}
	return false;
}


bool APBInst_Bullet::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}

void APBInst_Bullet::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = BulletData.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = BulletData.BulletHitDamage;

	if (Impact.GetActor())
	{
		AController* PawnController = Instigator ? Instigator->Controller : nullptr;
		float ActualDamage = Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, PawnController, this);

		// deal kills
		TArray<FTakeDamageInfo>	TakeDamageInfos;
		FTakeDamageInfo info;
		info.Victim = Impact.GetActor();
		info.ActualDamage = ActualDamage;
		info.ItemId = this->WeaponId;

		APBCharacter* Character = Cast<APBCharacter>(Impact.GetActor());
		if (Character)
		{
			info.bIsHeadShot = Character->IsHeadShot(PointDmg);
		}

		TakeDamageInfos.Add(info);

		DealKills(PawnController, TakeDamageInfos, PointDmg);
	}
}

void APBInst_Bullet::DealDamage(const FPierceHitResult& PierceImpact, const FVector& ShootDir, float FinalDamageReduce)
{
	if (!PierceImpact.Impact.GetActor()->bCanBeDamaged)
		return;

	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = BulletData.DamageType;
	PointDmg.HitInfo = PierceImpact.Impact;
	PointDmg.ShotDirection = ShootDir;
	
	// 최종 Damage 계산 (최종 감소율을 곱할때 0 ~ 1 사이의 값만 허용하기 때문에 Clamp를 해준다)
	// 최종 return 값은 Clamp 를 하기 전의 값이다.
	// 첫번째 히트 대상은 대미지 감소율 수치를 적용하지 않는다.
	if (PierceImpact.IsFirstHit == false)
	{
		PointDmg.Damage = BulletData.BulletHitDamage * FMath::Clamp(FinalDamageReduce, 0.0f, 1.0f);
	}
	else
	{
		PointDmg.Damage = BulletData.BulletHitDamage;
	}

	if (PierceImpact.Impact.GetActor())
	{
		AController* PawnController = Instigator ? Instigator->Controller : nullptr;

		float ActualDamage = PierceImpact.Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, PawnController, this);

		// deal kills
		TArray<FTakeDamageInfo>	TakeDamageInfos;
		FTakeDamageInfo info;
		info.Victim = PierceImpact.Impact.GetActor();
		info.ActualDamage = ActualDamage;
		info.ItemId = this->WeaponId;

		APBCharacter* Character = Cast<APBCharacter>(PierceImpact.Impact.GetActor());
		if (Character)
		{
			info.bIsHeadShot = Character->IsHeadShot(PointDmg);
		}

		TakeDamageInfos.Add(info);

		DealKills(PawnController, TakeDamageInfos, PointDmg);
	}
}

void APBInst_Bullet::DealKills(AController* Killer, const TArray<FTakeDamageInfo>& TakeDamageInfos, FDamageEvent const& DamageEvent)
{
	if (Role != ROLE_Authority)
	{
		return;
	}

	TArray<AController*> KilledControllers;

	for (auto info : TakeDamageInfos)
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
		if (GetWorld())
		{
			auto GM = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
			if (GM)
			{
				GM->UpdateKillMark(Killer, KilledControllers, this, TakeDamageInfos);
			}			
		}		
	}

}

void APBInst_Bullet::PlayPierceFXClientRemote(const TArray<FPierceHitResult>& PierceHitResult, const FVector& ShootPoint, const FVector& ShootDir)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		PierceHitNotify.Origin = ShootPoint;
		PierceHitNotify.ShootDir = ShootDir;
		PierceHitNotify.PierceHitResult = PierceHitResult;
	}
}