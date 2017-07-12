// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeapon.h"
#include "PBWeap_DefuseKit.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBWeap_DefuseKit : public APBWeapon
{
	GENERATED_BODY()
	
public:

	APBWeap_DefuseKit();

	void PostInitializeComponents() override;

	// Call in server
	void SetTargetBomb(class APBProj_Bomb* Target);

	// Valid everywhere
	APBProj_Bomb* GetTargetBomb();

	bool CanDefuseBomb();

	virtual void FireWeapon(EWeaponPoint ePoint) override;

	UFUNCTION(reliable, server, WithValidation)
	void ServerFireWeapon(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir);

	UFUNCTION(reliable, client)
	void ClientConfirmBombDefused();

protected:
	
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BombToDefuse)
	TWeakObjectPtr<APBProj_Bomb> BombToDefuse;
	UFUNCTION()
	void OnRep_BombToDefuse();

};
