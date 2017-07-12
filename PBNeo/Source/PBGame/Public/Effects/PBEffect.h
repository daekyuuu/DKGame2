// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PBTypes.h"
#include "PBEffect.generated.h"


USTRUCT()
struct FDecalData
{
	GENERATED_BODY()

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* DefaultMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* ConcreteMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* DirtMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* WaterMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* MetalMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* WoodMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* GrassMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* GlassMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
	UMaterialInterface* FleshMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
		UMaterialInterface* CarpetMaterial;

	/** material */
	UPROPERTY(EditAnywhere, Category = Decal)
		UMaterialInterface* PaperMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	FVector DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float LifeSpan;

	UMaterialInterface* GetMaterial(TEnumAsByte<EPhysicalSurface> SurfaceType);

	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UCLASS(HideDropdown)
class PBGAME_API APBEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBEffect();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:

	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	FDecalData DecalData;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = "Surface")
	FHitResult SurfaceHit;

protected:

	/**
	* Spawn Decal at given point.
	* Decal will be choosed by Surface.
	*/
	virtual UDecalComponent* SpawnPBDecalAttached(const FVector& Location, const FRotator& Rotation);	
};
