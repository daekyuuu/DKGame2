// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBExplosionEffect.h"


// Sets default values
APBExplosionEffect::APBExplosionEffect()
{
	/*ExplosionParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionParticleComp"));
	ExplosionParticleComp->bAutoActivate = true;
	ExplosionParticleComp->bAutoDestroy = true;
	ExplosionParticleComp->AttachParent = RootComponent;*/

	//ExplosionLightComponentName = TEXT("ExplosionLight");

	//ExplosionLight = CreateDefaultSubobject<UPointLightComponent>(ExplosionLightComponentName);
	//RootComponent = ExplosionLight;
	//ExplosionLight->AttenuationRadius = 400.0;
	//ExplosionLight->Intensity = 500.0f;
	//ExplosionLight->bUseInverseSquaredFalloff = false;
	//ExplosionLight->LightColor = FColor(255, 185, 35);
	//ExplosionLight->CastShadows = false;
	//ExplosionLight->bVisible = true;

	//ExplosionLightFadeOut = 0.2f;

	ParticleScale = 1.0f;
}

// Called when the game starts or when spawned
void APBExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	
}

void APBExplosionEffect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APBExplosionEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	if (ExplosionFX)
	{
		auto Comp = UPBGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
		Comp->SetRelativeScale3D(FVector(ParticleScale, ParticleScale, ParticleScale));
	}


	if (ExplosionSound)
	{
		UPBGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	// spawn decal
	FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
	RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);
	SpawnPBDecalAttached(SurfaceHit.ImpactPoint, RandomDecalRotation);
}
