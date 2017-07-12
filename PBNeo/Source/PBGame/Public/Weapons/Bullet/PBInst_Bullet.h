// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeaponTypes.h"
#include "Weapons/PBWeaponData.h"
#include "Weapons/Bullet/PBBulletBase.h"
#include "PBInst_Bullet.generated.h"

struct FPierceHitResult;

USTRUCT(Blueprintable)
struct FBulletData
{
	GENERATED_USTRUCT_BODY()

	// bullet damage or melee attack damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet, meta = (DisplayName = "BulletRange(Table)"))
	float BulletRange;

	// bullet damage or melee attack damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet, meta = (DisplayName = "BulletDamage(Table)"))
	int32 BulletHitDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet, meta = (DisplayName = "BulletDamageMin(Table)"))
	int32 BulletDamageMin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet, meta = (DisplayName = "BulletDamageSlope(Table)"))
	float BulletDamageSlope;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet, meta = (DisplayName = "BulletDamageSlopeMinRange(Table)"))
	float BulletDamageSlopeMinRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet)
	TSubclassOf<UDamageType> DamageType;

	FBulletData()
	{
		BulletRange = 10000.f;
		BulletHitDamage = 0;
		DamageType = UDamageType::StaticClass();

	}

	bool ImportTableData(const FPBWeaponTableData* Data, bool IsDirty = false, bool IsMelee = false)
	{
		bool bIsDirty = false;

		if (!IsMelee)
		{
			if ((BulletRange != Data->BulletRange) && !bIsDirty) { bIsDirty = true; }
			BulletRange = (float)Data->BulletRange;

			if ((BulletHitDamage != Data->BulletDamage) && !bIsDirty) { bIsDirty = true; }
			BulletHitDamage = Data->BulletDamage;

			if ((BulletDamageMin != Data->BulletDamageMin) && !bIsDirty) { bIsDirty = true; }
			BulletDamageMin = Data->BulletDamageMin;

			if ((BulletDamageSlope != Data->BulletDamageSlope) && !bIsDirty) { bIsDirty = true; }
			BulletDamageSlope = Data->BulletDamageSlope;

			if ((BulletDamageSlopeMinRange != Data->BulletDamageSlopeMinRange) && !bIsDirty) { bIsDirty = true; }
			BulletDamageSlopeMinRange = Data->BulletDamageSlopeMinRange;
		}
		else
		{
			if ((BulletRange != Data->MeleeAttackRange) && !bIsDirty) { bIsDirty = true; }
			BulletRange = Data->MeleeAttackRange;

			if ((BulletHitDamage != Data->MeleeAttackDamage) && !bIsDirty) { bIsDirty = true; }
			BulletHitDamage = Data->MeleeAttackDamage;
		}

		return bIsDirty;
	}
};

UCLASS(HideDropdown)
class PBGAME_API APBInst_Bullet : public APBBulletBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBInst_Bullet();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	virtual bool ApplyTableData(const struct FPBWeaponTableData* Data);

	virtual void PointHit_Confirm(const FHitResult& Impact, const FVector& ShootPoint, const FVector& ShootDir);
	virtual float PointHit_Confirm(const FPierceHitResult& PierceImpact, const FVector& ShootPoint, const FVector& ShootDir, float PiercingPower);
	virtual void PointHit_Miss(const FVector& ShootPoint, const FVector& ShootDir);

	virtual bool ShouldDealDamage(AActor* TestActor) const override;

	virtual void DealDamage(const FHitResult& Impact, const FVector& ShootDir) override;
	
	/** [server call only] handle kills if exist. It should be called after apply damage like DealDamage() */
	virtual void DealKills(AController* Killer, const TArray<FTakeDamageInfo>& TakeDamageInfos, FDamageEvent const& DamageEvent);

	virtual void SpawnImpactEffects(const FHitResult& Impact) override;

	virtual void SpawnTrailEffect(const FVector& StartPoint, const FVector& EndPoint);

	virtual bool TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, EAttackDirType AttackDirType, FHitResult& OutHit);

	virtual bool TraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, EAttackDirType AttackDirType, TArray<FHitResult>& OutHits);

	void DealDamage(const FPierceHitResult& PierceImpact, const FVector& ShootDir, float FinalDamageReduce);

	void PlayPierceFXClientRemote(const TArray<FPierceHitResult>& PierceHitResult, const FVector& ShootPoint, const FVector& ShootDir);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FBulletData BulletData;	

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FBulletFX BulletFX;

	UPROPERTY(Instanced, EditDefaultsOnly, Category = HitTrace, meta = (DisplayName = "WeaponTrace"))
	class UPBWeapLineTrace* HitTrace;

	UFUNCTION()
	virtual void OnRep_HitNotify();

	UFUNCTION()
	void OnRep_PierceHitNotify();

	/** instant hit notify for replication */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PierceHitNotify)
	FInstantPierceHitInfo PierceHitNotify;

};
