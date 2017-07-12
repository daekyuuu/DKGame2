// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBEffect.h"
#include "GameFramework/Actor.h"
#include "PBExplosionEffect.generated.h"

UCLASS(HideDropdown, Blueprintable)
class PBGAME_API APBExplosionEffect : public APBEffect
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBExplosionEffect();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** spawn effect */
	virtual void PostInitializeComponents() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	UParticleSystem* ExplosionFX;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float ParticleScale;

	

};

