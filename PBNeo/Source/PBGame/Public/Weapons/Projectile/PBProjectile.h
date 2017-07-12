// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Bullet/PBBulletBase.h"
#include "PBProjectile.generated.h"

#define FORCE_TIMEOUT 15.f;



USTRUCT(Blueprintable)
struct FProjectileData
{
	GENERATED_USTRUCT_BODY()	

	// Meaning of Pending Expiration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float ProjectileLife;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float ExplosionDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (DisplayName = "ExplosionDamage(Table)"))
	int32 ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (DisplayName = "ExplosionRadius(Table)"))
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float PointHitDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	float PointHitPhysicsImpulse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	TSubclassOf<UDamageType> DamageType;

	FProjectileData()
	{
		ProjectileLife = 10.0f;
		ExplosionDuration = 0.f;
		ExplosionDamage = 100;
		ExplosionRadius = 300.0f;
		PointHitDamage = 10.0f;
		PointHitPhysicsImpulse = 10.0f;
		DamageType = UDamageType::StaticClass();
	}
};

/**
* 프로젝타일 최상위 클래스 
*/

UCLASS(Abstract, Blueprintable)
class PBGAME_API APBProjectile : public APBBulletBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBProjectile();

	/** initial setup */
	virtual void PostInitializeComponents() override;
	virtual void InitialLifeSpan();
	virtual void LifeSpanExpired() override;
	virtual void OnRep_Instigator() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	virtual bool ApplyTableData(const struct FPBWeaponTableData* Data);

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnBounce(const FHitResult& HitResult, const FVector& OldVelocity);

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnPointHit(const FHitResult& HitResult) override;

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnStop(const FHitResult& HitResult);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void DealDamage(const FHitResult& Impact, const FVector& ShootDir) override;

	/** [server call only] handle kills if exist. It should be called after apply damage like DealDamage() */
	virtual void ReportKills(AController* Killer, const TArray<FTakeDamageInfo>& TakeDamageInfos, FDamageEvent const& DamageEvent);

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);
	bool IsPendingDestroy();

	// 이 시간 되면 아무 효과없이 무조건 삭제한다 (사용하고 싶지 않다면 타이머를 -1.0f 로 설정)
	virtual void FailSafeForceDestroy();

protected:

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void ApplyImpulse(const FHitResult& HitResult) override;
	virtual void SpawnImpactEffects(const FHitResult& HitResult) override;
	virtual void Stop();
	
	//virtual 로 선언하지말것 (Don't define to virtual function)
	void WantsDestroy(bool bHideImmediately=true, float PendingDestroy=0.5f);

	// 디버그 라인 표시해주는 플래그인데 이건 나중에 콘솔명령으로 변경 예정임.
	UPROPERTY(EditDefaultsOnly, Category = DebugConfig)
	bool bShowDebugLine;

	// It is a safeguard to prevent the problem that the object does not disappear
	UPROPERTY(EditDefaultsOnly, Category = Config, meta = (DisplayName = "FailsafeForceDestroyTimer(no effect)"))
	float FailsafeForceDestroyTimer;

	FTimerHandle TimerHandle_ForceDestroy;

	bool bInitializedLifeSpan;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FProjectileData ProjData;

	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> InstigatorController;

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* ProjMesh;

	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Effects)
	UParticleSystemComponent* ParticleComp;

	/** impact effects */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBImpactEffect> ImpactTemplate;
	
	FHitResult SavedHitResult;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 MinBounceNum;

	int32 CurrentBounceNum;

	bool bWantsDestroy;

	bool bStopped;

public:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }

public:
	const struct FProjectileData& GetProjectileData() const;
};
