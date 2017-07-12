// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Pickups/PBPickup.h"
#include "Pickups/PBPickupRandomSpawner.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PBCharacter.h"
#include "PBActionPickup.h"



// Sets default values
APBPickup::APBPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PBPickup"));

	// Initialize the pickup volumes
	InnerVolumeSize = 200.0f;
	OuterVolumeSize = 1000.0f;
	InitVolumes();

	PickupMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->CastShadow = false;
	PickupMesh->SetupAttachment(RootComponent);

	MoveRotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MoveRotatingComp"));
	MoveRotatingComp->RotationRate.Yaw = 90.f;

	PickupPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickupFX"));
	PickupPSC->bAutoActivate = false;
	PickupPSC->bAutoDestroy = false;
	PickupPSC->SetupAttachment(RootComponent);

	bActive = false;
	PickedUpBy = nullptr;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	SetReplicateMovement(true);

	RefPickupRandomSpawner = nullptr;


}

void APBPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPickup, bActive);
	DOREPLIFETIME(APBPickup, PickedUpBy);
}

// Called when the game starts or when spawned
void APBPickup::BeginPlay()
{
	Super::BeginPlay();

	RespawnElem.SpawnNum = 0;
	GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Respawn, this, &APBPickup::Spawn, RespawnElem.InitialSpawnDelay, false);
	GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Destroy, this, &APBPickup::Reset, RespawnElem.DestroyDelay, false);
}

// Called every frame
void APBPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APBPickup::InitVolumes()
{
	InnerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InnerVolume"));
	InnerVolume->bGenerateOverlapEvents = true;
	InnerVolume->SetSphereRadius(InnerVolumeSize);
	InnerVolume->SetCollisionProfileName("Pickup");
	InnerVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &APBPickup::OnInnerBeginOverlap);
	InnerVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &APBPickup::OnInnerEndOverlap);
	InnerVolume->SetupAttachment(GetRootComponent());

	OuterVolume = CreateDefaultSubobject<USphereComponent>(TEXT("OuterVolume"));
	OuterVolume->bGenerateOverlapEvents = true;
	OuterVolume->SetSphereRadius(OuterVolumeSize);
	OuterVolume->SetCollisionProfileName("Pickup");
	OuterVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &APBPickup::OnOuterBeginOverlap);
	OuterVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &APBPickup::OnOuterEndOverlap);
	OuterVolume->SetupAttachment(GetRootComponent());

}

void APBPickup::OnInnerBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto ActionPickup = GetActionPickupFromActor(OtherActor);
	if (ActionPickup)
	{
		ActionPickup->NotifyInnerBeginOverlap(this);
	}
}

void APBPickup::OnInnerEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto ActionPickup = GetActionPickupFromActor(OtherActor);
	if (ActionPickup)
	{
		ActionPickup->NotifyInnerEndOverlap(this);
	}

}

void APBPickup::OnOuterBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto ActionPickup = GetActionPickupFromActor(OtherActor);
	if (ActionPickup)
	{
		ActionPickup->NotifyOuterBeginOverlap(this);
	}

}

void APBPickup::OnOuterEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto ActionPickup = GetActionPickupFromActor(OtherActor);
	if (ActionPickup)
	{
		ActionPickup->NotifyOuterEndOverlap(this);
	}

}

class UPBActionPickup* APBPickup::GetActionPickupFromActor(class AActor* OtherActor)
{
	if (Role == ROLE_Authority)
	{
		APBCharacter* PBCharacter = Cast<APBCharacter>(OtherActor);
		if (PBCharacter)
		{
			return PBCharacter->GetActionPickup();
		}
	}

	return nullptr;
}

// Called at APBPickupRandomSpawner::Reset() when the game starts or round restart
void APBPickup::Reset()
{
	bActive = false;
	PickedUpBy = nullptr;
	RefPickupRandomSpawner = nullptr;
	RespawnElem.SpawnNum = 0;
	GetWorldTimerManager().ClearTimer(RespawnElem.TimerHandle_Respawn);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (PickupPSC)
	{
		PickupPSC->DeactivateSystem();
	}		
}

bool APBPickup::IsActive()
{
	return bActive;
}

void APBPickup::SetActive(bool bSet)
{
	bActive = bSet;
}

void APBPickup::OnRep_IsActive()
{
	if (bActive)
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SpawnEffects();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		PickupEffects();
	}
}

// Called at APBPickupRandomSpawner::Spawn (server only)
void APBPickup::SpawnerSpawn(class APBPickupRandomSpawner* Spawner)
{
	RefPickupRandomSpawner = Spawner;

	Spawn();

	// 스포너에서 제어하므로 자체 타이머는 제거한다.
	GetWorldTimerManager().ClearTimer(RespawnElem.TimerHandle_Respawn);
}

void APBPickup::Spawn()
{
	if (bActive)
	{
		return;
	}

	bActive = true;
	PickedUpBy = nullptr;
	RespawnElem.SpawnNum++;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SpawnEffects();
}

bool APBPickup::CanPickup(APBCharacter* Player)
{
	// To prevent the pickup fighting between players
	if (PickedUpBy != nullptr)
		return false;
	
	if (!bActive || IsPendingKill())
		return false;

	if (Player == nullptr || !Player->IsAlive() || Player->bTearOff)
		return false;

	return true;
}

// server only
void APBPickup::PickupOnTouch(APBCharacter* Player)
{
	if (Role != ROLE_Authority)
		return;

	if (CanPickup(Player))
	{
		GivePickupTo(Player);

		PickedUpBy = Player;	//Replicated for client pickup
		bActive = false;		//Replicated for client pickup

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		PickupEffects();

		// in case spawn by RandomSpawner
		if (RefPickupRandomSpawner)
		{
			RefPickupRandomSpawner->OnPickedUp(this);
			RefPickupRandomSpawner = nullptr;
		}
		else
		{
			if (RespawnElem.CanSpawn())
			{
				GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Respawn, this, &APBPickup::Spawn, RespawnElem.NextSpawnDelay, false);				
			}			
		}		
	}
}

void APBPickup::SetMaxSpawnNum(int32 Num)
{
	RespawnElem.MaxSpawnNum = Num;
}

void APBPickup::SetDestroyDelay(float Delay)
{
	RespawnElem.DestroyDelay = Delay;
}

// server only
void APBPickup::GivePickupTo(APBCharacter* Player)
{
	//Todo in child
}

void APBPickup::SpawnEffects()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (ActiveFX)
		{
			PickupPSC->SetTemplate(ActiveFX);
			PickupPSC->ActivateSystem();
		}
		else
		{
			PickupPSC->DeactivateSystem();
		}

		if (SpawnStartFX)
		{
			UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(SpawnStartFX, RootComponent, NAME_None, SpawnStartFXTransform.GetLocation(), SpawnStartFXTransform.GetRotation().Rotator());

			if (PSC != NULL)
			{
				PSC->SetRelativeScale3D(SpawnStartFXTransform.GetScale3D());
			}
		}

		const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const bool bJustSpawned = CreationTime <= (CurrentTime + 5.0f);
		if (SpawnSound && !bJustSpawned)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
		}
	}

	//Call the Blueprint event
	OnSpawnEvent();
}

void APBPickup::PickupEffects()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (PreActiveFX)
		{
			PickupPSC->SetTemplate(PreActiveFX);
			PickupPSC->ActivateSystem();
		}
		else
		{
			PickupPSC->DeactivateSystem();
		}

		if (PickupSound && PickedUpBy)
		{
			UGameplayStatics::SpawnSoundAttached(PickupSound, PickedUpBy->GetRootComponent());
		}

		if (PickupStartFX && PickedUpBy)
		{
			UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(PickupStartFX, RootComponent, NAME_None, PickupStartFXTransform.GetLocation(), PickupStartFXTransform.GetRotation().Rotator());
			if (PSC != NULL)
			{
				PSC->SetRelativeScale3D(PickupStartFXTransform.GetScale3D());
			}
		}
	}

	//Call the Blueprint event
	OnPickedUpEvent();

	
}
