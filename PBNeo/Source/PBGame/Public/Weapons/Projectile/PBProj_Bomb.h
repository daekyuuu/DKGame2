// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Projectile/PBProj_Explosive.h"
#include "PBProj_Bomb.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBProj_Bomb : public APBProj_Explosive
{
	GENERATED_BODY()

public:

	APBProj_Bomb();

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USphereComponent* BombDefuseVolume;

	void BeginPlay() override;

	virtual void OnRep_Instigator() override;

	//void SetupSecondsUntilExplosion();

	// Call in server only - actually defused by someone (may have effect/sound)
	void Defuse();

	// Call in server only - make sure the bomb will not do anything
	void FreezeBomb();

	void Explode(const FHitResult& Impact) override;

protected:

	UFUNCTION()
	void OnOverlapDefuseVolume(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnUnOverlapDefuseVolume(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Call only on server
	void SetIsDefused(bool Value);
	UPROPERTY(Transient, ReplicatedUsing= OnRep_IsDefused)
	bool bIsDefused;
	UFUNCTION()
	void OnRep_IsDefused();

	UFUNCTION(BlueprintNativeEvent, Category = Bomb)
	void OnBombDefused();

	// Call only on server
	void SetIsFrozen(bool Value);
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsFrozen)
	bool bIsFrozen;
	UFUNCTION()
	void OnRep_IsFrozen();

	UFUNCTION(BlueprintNativeEvent, Category = Bomb)
	void OnBombFrozen();	

	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerSetSecondsUntilExplosion(float InSecondsUntilExplosion);

	//UPROPERTY(ReplicatedUsing = OnRep_SecondsUntilExplosion)
	//float SecondsUntilExplosion;

	//UFUNCTION()
	//virtual void OnRep_SecondsUntilExplosion();
};
