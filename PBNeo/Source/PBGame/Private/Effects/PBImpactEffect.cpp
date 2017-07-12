// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBImpactEffect.h"
#include "Sound/SoundNodeLocalPlayer.h"

// Sets default values
APBImpactEffect::APBImpactEffect()
{
	ParticleScale = 1.0f;
}

// Called when the game starts or when spawned
void APBImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APBImpactEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APBImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		auto Comp = UPBGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
		Comp->SetRelativeScale3D(FVector(ParticleScale, ParticleScale, ParticleScale));
	}

	// play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		APBCharacter* Pawn = Cast<APBCharacter>(GetInstigator());
		if (Pawn)
		{
			UPBGameplayStatics::SpawnSoundAtLocation(this, Pawn, ImpactSound, GetActorLocation());
		}
		else
		{
			UPBGameplayStatics::SpawnSoundAtLocation(this, nullptr, ImpactSound, GetActorLocation());
		}
	}

	// spawn decal
	FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
	RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);
	SpawnPBDecalAttached(SurfaceHit.ImpactPoint, RandomDecalRotation);
}

UParticleSystem* APBImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
	case PB_SURFACE_Concrete:	ImpactFX = ConcreteFX; break;
	case PB_SURFACE_Dirt:		ImpactFX = DirtFX; break;
	case PB_SURFACE_Water:		ImpactFX = WaterFX; break;
	case PB_SURFACE_Metal:		ImpactFX = MetalFX; break;
	case PB_SURFACE_Wood:		ImpactFX = WoodFX; break;
	case PB_SURFACE_Grass:		ImpactFX = GrassFX; break;
	case PB_SURFACE_Glass:		ImpactFX = GlassFX; break;
	case PB_SURFACE_Flesh:		ImpactFX = FleshFX; break;
	case PB_SURFACE_Carpet:		ImpactFX = CarpetFX; break;
	case PB_SURFACE_Paper:		ImpactFX = PaperFX; break;
	case PB_SURFACE_Head:		ImpactFX = HeadFX; break;

	default:					ImpactFX = DefaultFX; break;
	}

	if (ImpactFX == nullptr)
		ImpactFX = DefaultFX;

	return ImpactFX;
}

USoundCue* APBImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType)
{
	USoundCue* ImpactSound = NULL;

	switch (SurfaceType)
	{
	case PB_SURFACE_Concrete:	ImpactSound = ConcreteSound; break;
	case PB_SURFACE_Dirt:		ImpactSound = DirtSound; break;
	case PB_SURFACE_Water:		ImpactSound = WaterSound; break;
	case PB_SURFACE_Metal:		ImpactSound = MetalSound; break;
	case PB_SURFACE_Wood:		ImpactSound = WoodSound; break;
	case PB_SURFACE_Grass:		ImpactSound = GrassSound; break;
	case PB_SURFACE_Glass:		ImpactSound = GlassSound; break;
	case PB_SURFACE_Flesh:		ImpactSound = FleshSound; break;
	case PB_SURFACE_Carpet:		ImpactSound = CarpetSound; break;
	case PB_SURFACE_Paper:		ImpactSound = PaperSound; break;
	case PB_SURFACE_Head:		ImpactSound = HeadSound; break;

	default:					ImpactSound = DefaultSound; break;
	}

	if (ImpactSound == nullptr)
		ImpactSound = DefaultSound;

	return ImpactSound;
}