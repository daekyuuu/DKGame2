// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickups/PBPickupSpawnTypes.h"
#include "PBPickup.generated.h"

class APBCharacter;
class APBPickupRandomSpawner;
class UPBActionPickup;

UCLASS(abstract)
class APBPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APBPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/* ------------------------------------------------------------------------------- */
	// Inner, Outer volumes
	/* ------------------------------------------------------------------------------- */

protected:

	UPROPERTY()
	USphereComponent* InnerVolume;

	UPROPERTY()
	USphereComponent* OuterVolume;

	void InitVolumes();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
		float InnerVolumeSize;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
		float OuterVolumeSize;


private:

	UFUNCTION()
	virtual void OnInnerBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnInnerEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnOuterBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOuterEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	class UPBActionPickup* GetActionPickupFromActor(class AActor* OtherActor);

public:

	// Called at APBPickupRandomSpawner::Reset() when round restart
	virtual void Reset();

	bool IsActive();
	void SetActive(bool bSet);

	virtual void SpawnerSpawn(class APBPickupRandomSpawner* Spawner);
	virtual void Spawn();

	virtual bool CanPickup(APBCharacter* Player);

	virtual void PickupOnTouch(APBCharacter* Player);

	void SetMaxSpawnNum(int32 Num);
	void SetDestroyDelay(float Delay);

protected:	

	virtual void GivePickupTo(APBCharacter* Player);
	
	virtual void SpawnEffects();
	virtual void PickupEffects();

	// blueprint event: pickup disappears
	UFUNCTION(BlueprintImplementableEvent)
	void OnPickedUpEvent();

	// blueprint event: pickup appears
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpawnEvent();

protected:

	/** is it ready for interactions? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
	uint32 bActive : 1;

	/* The character who has picked up this pickup */
	UPROPERTY(Transient, Replicated)
	APBCharacter* PickedUpBy;

	APBPickupRandomSpawner* RefPickupRandomSpawner;

	UPROPERTY()
	FPickupRespawnElem RespawnElem;

	UFUNCTION()
	void OnRep_IsActive();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	USkeletalMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	URotatingMovementComponent* MoveRotatingComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Effects)
	UParticleSystemComponent* PickupPSC;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* PickupStartFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	FTransform PickupStartFXTransform;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* PickupSound;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* PreActiveFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* ActiveFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* SpawnStartFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FTransform SpawnStartFXTransform;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* SpawnSound;
};
