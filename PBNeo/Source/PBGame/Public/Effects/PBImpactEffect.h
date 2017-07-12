// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBEffect.h"
#include "GameFramework/Actor.h"
#include "PBImpactEffect.generated.h"

UCLASS(HideDropdown, Blueprintable)
class PBGAME_API APBImpactEffect : public APBEffect
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBImpactEffect();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** default impact FX used when material specific override doesn't exist */

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float ParticleScale;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	UParticleSystem* DefaultFX;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* ConcreteFX;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* DirtFX;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* WaterFX;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* MetalFX;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* WoodFX;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* GlassFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* GrassFX;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* FleshFX;

	/** impact FX on head */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* HeadFX;

	/** impact FX on Carpet */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* CarpetFX;

	/** impact FX on Paper */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* PaperFX;

	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	USoundCue* DefaultSound;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ConcreteSound;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* DirtSound;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* WaterSound;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* MetalSound;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* WoodSound;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* GlassSound;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* GrassSound;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FleshSound;

	/** impact FX on head */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* HeadSound;

	/** impact FX on Carpet */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* CarpetSound;

	/** impact FX on Paper */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* PaperSound;

	/** spawn effect */
	virtual void PostInitializeComponents() override;

protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType);
	
};
