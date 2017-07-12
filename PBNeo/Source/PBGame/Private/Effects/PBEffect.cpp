// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBEffect.h"


UMaterialInterface* FDecalData::GetMaterial(TEnumAsByte<EPhysicalSurface> SurfaceType)
{
	UMaterialInterface* mtrl = NULL;

	switch (SurfaceType)
	{
	case PB_SURFACE_Concrete:	mtrl = ConcreteMaterial; break;
	case PB_SURFACE_Dirt:		mtrl = DirtMaterial; break;
	case PB_SURFACE_Water:		mtrl = WaterMaterial; break;
	case PB_SURFACE_Metal:		mtrl = MetalMaterial; break;
	case PB_SURFACE_Wood:		mtrl = WoodMaterial; break;
	case PB_SURFACE_Grass:		mtrl = GrassMaterial; break;
	case PB_SURFACE_Glass:		mtrl = GlassMaterial; break;
	case PB_SURFACE_Flesh:		mtrl = FleshMaterial; break;
	case PB_SURFACE_Carpet:		mtrl = CarpetMaterial; break;
	case PB_SURFACE_Paper:		mtrl = PaperMaterial; break;

	default:					mtrl = DefaultMaterial; break;
	}

	if (mtrl == nullptr)
		return DefaultMaterial;

	return mtrl;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Sets default values
APBEffect::APBEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAutoDestroyWhenFinished = true;	
}

// Called when the game starts or when spawned
void APBEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APBEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

UDecalComponent* APBEffect::SpawnPBDecalAttached(const FVector& Location, const FRotator& Rotation)
{
	// Get Surface type.
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	if (SurfaceHit.PhysMaterial.IsValid())
	{
		SurfaceType = SurfaceHit.PhysMaterial->SurfaceType;
	}


	// Get Decal material from Surface type
	UMaterialInterface* DecalMtrl = DecalData.GetMaterial(SurfaceType);

	// Spawn Decal.
	UDecalComponent* SpawnedDecal = nullptr;
	if (DecalMtrl && SurfaceHit.Component.Get())
	{
		SpawnedDecal = UPBGameplayStatics::SpawnDecalAttached(DecalMtrl, DecalData.DecalSize,
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			Location, Rotation, EAttachLocation::KeepWorldPosition,
			DecalData.LifeSpan);

		// I Will not replicate the Effect.
		if (SpawnedDecal)
		{
			SpawnedDecal->SetIsReplicated(false);
		}
	}

	return SpawnedDecal;
}
