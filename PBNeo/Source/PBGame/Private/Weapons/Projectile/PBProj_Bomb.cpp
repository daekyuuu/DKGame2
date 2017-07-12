// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBProj_Bomb.h"
#include "PBMode_BombMission.h"
#include "PBTrigger_BombSite.h"

APBProj_Bomb::APBProj_Bomb()
{
	//@note: for networking
	{
		bAlwaysRelevant = true;
	}

	BombDefuseVolume = CreateDefaultSubobject<USphereComponent>(TEXT("BombDiffusionVolume"));
	BombDefuseVolume->AlwaysLoadOnClient = true;
	BombDefuseVolume->AlwaysLoadOnServer = true;
	BombDefuseVolume->SetCollisionProfileName(TEXT("PlayerTrigger"));
	BombDefuseVolume->SetupAttachment(RootComponent);
	BombDefuseVolume->OnComponentBeginOverlap.AddDynamic(this, &APBProj_Bomb::OnOverlapDefuseVolume);
	BombDefuseVolume->OnComponentEndOverlap.AddDynamic(this, &APBProj_Bomb::OnUnOverlapDefuseVolume);
}

void APBProj_Bomb::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBProj_Bomb, bIsDefused);
	DOREPLIFETIME(APBProj_Bomb, bIsFrozen);
	//DOREPLIFETIME(APBProj_Bomb, SecondsUntilExplosion);
}

void APBProj_Bomb::BeginPlay()
{
	Super::BeginPlay();

	auto World = GetWorld();
	if (World)
	{
		if (Role == ROLE_Authority)
		{
			auto GS = World->GetGameState<APBGameState>();
			if (GS)
			{
				GS->ServerSetBombRemainingTime(ProjData.ProjectileLife);
			}

			auto C = Cast<APBCharacter>(Instigator);
			if (C)
			{
				auto BombSite = const_cast<APBTrigger_BombSite *>(C->GetOverlappedBombSite());
				BombSite->ServerSetIsPlantedBomb(true);
			}
		}
	}
}

void APBProj_Bomb::OnRep_Instigator()
{
	Super::OnRep_Instigator();

	auto World = GetWorld();
	if (World)
	{
		if (Role == ROLE_Authority)
		{
			auto C = Cast<APBCharacter>(Instigator);
			if (C)
			{
				auto BombSite = const_cast<APBTrigger_BombSite *>(C->GetOverlappedBombSite());
				BombSite->ServerSetIsPlantedBomb(true);
			}
		}
	}
}

void APBProj_Bomb::OnOverlapDefuseVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == BombDefuseVolume)
	{
		APBCharacter* EnteringCharacter = Cast<APBCharacter>(OtherActor);
		if (EnteringCharacter)
		{
			EnteringCharacter->NotifyEnterBombRadius(this);
		}
	}
}

void APBProj_Bomb::OnUnOverlapDefuseVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == BombDefuseVolume)
	{
		APBCharacter* EnteringCharacter = Cast<APBCharacter>(OtherActor);
		if (EnteringCharacter)
		{
			EnteringCharacter->NotifyLeaveBombRadius(this);
		}
	}
}

void APBProj_Bomb::Defuse()
{
	if (bIsFrozen)
	{
		// Frozen bomb does nothing
		return;
	}

	if (GetWorld() == nullptr)
	{
		return;
	}

	AGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AGameMode>() : nullptr;
	if (GM && !(GM->GetMatchState() == MatchState::InProgress))
	{
		// Do not explode when the game is not running
		return;
	}

	if (!bExploded && !bIsDefused)
	{
		SetIsDefused(true);

		if (GetWorld())
		{
			APBMode_BombMission* BombMode = Cast<APBMode_BombMission>(GetWorld()->GetAuthGameMode());
			if (BombMode)
			{
				BombMode->OnGlobalEvent(FName(TEXT("BombDefused")));
			}
		}
	}
}

void APBProj_Bomb::SetIsDefused(bool Value)
{
	if (bIsDefused != Value)
	{
		bIsDefused = Value;
		OnRep_IsDefused();
	}
}

void APBProj_Bomb::OnRep_IsDefused()
{
	OnBombDefused();

	auto World = GetWorld();
	if (bIsDefused && World)
	{
		if (Role == ROLE_Authority)
		{
			auto GS = World->GetGameState<APBGameState>();
			if (GS)
			{
				GS->ServerSetBombRemainingTime(0);
			}

			auto C = Cast<APBCharacter>(Instigator);
			if (C)
			{
				auto BombSite = const_cast<APBTrigger_BombSite *>(C->GetOverlappedBombSite());
				if (BombSite)
				{
					BombSite->ServerSetIsPlantedBomb(false);
				}
			}
		}

		for (auto It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APBPlayerController* PC = Cast<APBPlayerController>(*It);
			if (PC && PC->IsLocalController())
			{
				PC->WidgetEventDispatcher->OnBombDefused.Broadcast();
			}
		}
	}
}

void APBProj_Bomb::OnBombDefused_Implementation()
{
}

void APBProj_Bomb::FreezeBomb()
{
	SetIsFrozen(true);
}

void APBProj_Bomb::SetIsFrozen(bool Value)
{
	if (bIsFrozen != Value)
	{
		bIsFrozen = Value;
		OnRep_IsFrozen();
	}
}

void APBProj_Bomb::OnRep_IsFrozen()
{
	OnBombFrozen();

	auto World = GetWorld();
	if (bIsFrozen && World)
	{
		if (Role == ROLE_Authority)
		{
			auto GS = World->GetGameState<APBGameState>();
			if (GS)
			{
				GS->ServerSetBombRemainingTime(0);
			}

			auto C = Cast<APBCharacter>(Instigator);
			if (C)
			{
				auto BombSite = const_cast<APBTrigger_BombSite *>(C->GetOverlappedBombSite());
				if (BombSite)
				{
					BombSite->ServerSetIsPlantedBomb(false);
				}
			}
		}

		for (auto It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APBPlayerController* PC = Cast<APBPlayerController>(*It);
			if (PC && PC->IsLocalController())
			{
				PC->WidgetEventDispatcher->OnBombFrozen.Broadcast();
			}
		}
	}
}

void APBProj_Bomb::OnBombFrozen_Implementation()
{
}

void APBProj_Bomb::Explode(const FHitResult& Impact)
{
	if (bIsDefused || bIsFrozen)
		return;

	Super::Explode(Impact);

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (Role == ROLE_Authority)
	{
		APBMode_BombMission* GM = Cast<APBMode_BombMission>(World->GetAuthGameMode());
		if (GM)
		{
			GM->OnGlobalEvent(FName(TEXT("BombExploded")));
		}
	}

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APBPlayerController* PC = Cast<APBPlayerController>(*It);
		if (PC && PC->IsLocalController())
		{
			PC->WidgetEventDispatcher->OnBombExploded.Broadcast();
		}
	}
}