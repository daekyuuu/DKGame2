// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Projectile/PBProjectile.h"
#include "PBProj_Explosive.generated.h"

UENUM(BlueprintType)
enum class EProjExpirationType : uint8
{
	// Should Call 'ForceExplode' function
	ProgrammersForce,

	// ProjectileLife
	OnlyTimeout,

	// Needs setting 'MinBounceNum'
	OnlyBounce,

	// Same as OnlyBounce. However, you do not need to set 'MinBounceNum'
	OnlyFirstBounce,

	// both conditions (timer started, wait both timeout and bounce)
	BounceAndTimeout,

	// Either conditions (wait for either timeout or bounce)
	BounceOrTimeout,

	// Sequential conditions (bounce, next timer start)
	BounceToTimeout,	

	// Same as StopToTimeout. However, you do not need to set 'ProjectileLife'
	OnlyStop,

	// Sequential conditions (stop, next timer start)
	StopToTimeout,	

	// No further types are allowed (더 이상의 타입 추가는 불허함. Projectile 재정의 해서 사용하세요)
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PBGAME_API APBProj_Explosive : public APBProjectile
{
	GENERATED_BODY()	
	
public:
	APBProj_Explosive();

	virtual void LifeSpanExpired() override;
	virtual void PostInitializeComponents() override;

	virtual void InitialLifeSpan() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	EProjExpirationType ExpirationType;

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnBounce(const FHitResult& HitResult, const FVector& OldVelocity) override;

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnPointHit(const FHitResult& HitResult) override;

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnStop(const FHitResult& HitResult) override;

	//Self damaged(총에 맞으면 터지게 한다든가..)
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	virtual void DetermineExpired(bool bForce=false);
	virtual void Expired();

	/** trigger explosion */
	virtual void Explode(const FHitResult& Impact);

	UFUNCTION(BlueprintCallable, Category=Effects)
	void ForceExplode();

	/** [client] explosion happened */
	UFUNCTION()
	void OnRep_Exploded();

protected:

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APBExplosionEffect> ExplosionTemplate;

	/** did it explode? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Exploded)
	bool bExploded;		

	FHitResult& GetSpecificHitResult();	

};
