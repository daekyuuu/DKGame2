// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupRandomSpawner.h"
#include "Pickups/PBPickup.h"
#include "Mode/PBGameMode.h"
#include "Mode/PBGameState.h"


//////////////////////////////////////////////////////////////////////////

// Sets default values
APBPickupRandomSpawner::APBPickupRandomSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnedPickupItem = nullptr;
	SpawnedPickupItemType = nullptr;
	SpawnRule = EPickupSpawnRule::Random;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
}

void APBPickupRandomSpawner::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPickupRandomSpawner, SpawnedPickupItemType);
}

// Called when the game starts or when spawned
void APBPickupRandomSpawner::BeginPlay()
{
	Super::BeginPlay();
	Reset();
}

// Called when the game starts or round restarts
void APBPickupRandomSpawner::Reset()
{
	if (SpawnedPickupItem)
	{
		SpawnedPickupItem->Reset();
		SpawnedPickupItem = nullptr;
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Respawn, this, &APBPickupRandomSpawner::HandleSpawn, RespawnElem.InitialSpawnDelay, false);
	RespawnElem.SpawnNum = 0;

	Super::Reset();
}

// Called every frame
void APBPickupRandomSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

TSubclassOf<APBPickup> APBPickupRandomSpawner::GetSpawnTypeRandom()
{
	int32 TotalChance = 0;

	for (int32 i = 0; i < SpawnChances.Num(); i++)
	{
		if (SpawnChances[i].ItemType != nullptr)
			TotalChance += SpawnChances[i].Chance;
	}

	if (TotalChance == 0)
		return nullptr;

	// find spawn class
	int32 RandomChance = FMath::Rand() % TotalChance;
	TotalChance = 0;

	for (auto& Spawn : SpawnChances)
	{
		if (Spawn.ItemType == nullptr)
			continue;

		TotalChance += Spawn.Chance;

		if (RandomChance < TotalChance)
		{
			return Spawn.ItemType;
		}
	}

	return nullptr;
}

TSubclassOf<APBPickup> APBPickupRandomSpawner::GetSpawnTypeCycle()
{
	int32 ChanceNum = SpawnChances.Num();
	if (ChanceNum == 0)
		return nullptr;

	int32 CurrentSpawnIdx = 0;
	for (int32 i = 0; i < ChanceNum; i++)
	{
		if (SpawnChances[i].ItemType == SpawnedPickupItemType)
			CurrentSpawnIdx = i;
	}

	int32 NextSpawnIdx = (CurrentSpawnIdx + 1) % ChanceNum;
	int32 SafetyLoopCount = 0;

	// if ItemType is nullptr, find next to loop
	do 
	{
		if (SpawnChances[NextSpawnIdx].ItemType == nullptr)
		{
			NextSpawnIdx = (NextSpawnIdx + 1) % ChanceNum;
		}
		SafetyLoopCount++;

	} while (SpawnChances[NextSpawnIdx].ItemType == nullptr || SafetyLoopCount <= ChanceNum);

	return SpawnChances[NextSpawnIdx].ItemType;
}


// Called by timer handle
void APBPickupRandomSpawner::HandleSpawn()
{	
	if (Role == ROLE_Authority)
	{
		// 랜덤 or 순서대로
		TSubclassOf<APBPickup> WantsSpawnType = (SpawnRule == EPickupSpawnRule::Random) ? GetSpawnTypeRandom() : GetSpawnTypeCycle();
		
		// Case A: 만약 스폰 하려는 픽업 타입이 이미 스폰되어 활성화 상태이면 아무것도 하지 않는다.
		// Case B: 스폰 하려는 픽업 타입이 이미 스폰된 타입과 다른 경우 Reset() 호출을 통해 비활성화 시키고 새로 스폰한다.

		if (SpawnedPickupItem && SpawnedPickupItem->IsActive())
		{
			// Case A
			if (SpawnedPickupItem->GetClass() == WantsSpawnType)
			{
				return;
			}				

			// Case B
			SpawnedPickupItem->Reset();
		}

		// Case B
		OnSpawn(WantsSpawnType);
	}
}

bool APBPickupRandomSpawner::CanSpawn(TSubclassOf<APBPickup> WantsType)
{
	if (WantsType == nullptr)
	{
		return false;
	}

	if (!RespawnElem.CanSpawn())
	{
		return false;
	}

	return true;
}

// server only
void APBPickupRandomSpawner::OnSpawn(TSubclassOf<APBPickup> WantsType)
{
	if (GetWorld() == nullptr)
		return;

	if (!CanSpawn(WantsType))
	{
		return;
	}

	APBGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
	if (GameMode)
	{
		APBPickup* PickupItem = GameMode->NewPickupItem(WantsType, GetTransform());

		if (PickupItem)
		{
			PickupItem->SpawnerSpawn(this);

			SpawnedPickupItem = PickupItem;

			SpawnedPickupItemType = WantsType;

			RespawnElem.SpawnNum++;
		}
	}

	// Respawn
	GetWorldTimerManager().ClearTimer(RespawnElem.TimerHandle_Respawn);
	GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Respawn, this, &APBPickupRandomSpawner::HandleSpawn, RespawnElem.NextSpawnDelay, false);
}

// server only
void APBPickupRandomSpawner::OnPickedUp(APBPickup* Pickup)
{
	SpawnedPickupItem = nullptr;
	//SpawnedPickupItemType = nullptr; //don't set nullptr, used for next spawn

	GetWorldTimerManager().ClearTimer(RespawnElem.TimerHandle_Respawn);

	if (RespawnElem.NextSpawnDelay > 0.f)
	{		
		GetWorldTimerManager().SetTimer(RespawnElem.TimerHandle_Respawn, this, &APBPickupRandomSpawner::HandleSpawn, RespawnElem.NextSpawnDelay, false);
	}		
}
