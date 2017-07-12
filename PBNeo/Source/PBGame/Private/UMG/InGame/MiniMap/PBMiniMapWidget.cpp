// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMiniMapWidget.h"
#include "PBMiniMapCanvas.h"
#include "PBPlayerStart.h"
#include "PBTrigger_BombSite.h"

FMiniMapData UPBMiniMapWidget::GetMiniMapData()
{
	FMiniMapData Rtn;

	APBCharacter* MyCharacter = Cast<APBCharacter>(GetOwningPlayerPawn());
	if (!MyCharacter)
	{
		// There is no owning APBCharacter pawn as reference
		// There is no way for us to give any meaningful data
		return Rtn;
	}

	APBMiniMapCanvas* CanvasBelow = nullptr;
	APBMiniMapCanvas* CanvasAbove = nullptr; 
	if (!GetMiniMapCanvas(MyCharacter->GetActorLocation(), CanvasBelow, CanvasAbove))
	{
		// There is no canvas below to project the actors to
		// There is no way for us to give any meaningful data
		return Rtn;
	}

	// Fill the Single-instance data of the return value first
	{
		Rtn.DataIsValid = true;
		Rtn.CanvasExtent = CanvasBelow->GetExtent();
		Rtn.CanvasTexture = CanvasBelow->MiniMapTexture;
		Rtn.OwnInfo = GetPlayerData(MyCharacter, CanvasBelow);
	}

	// Fill the Per-Player data
	float IgnoreObjectsLowerThanThis = CanvasBelow->GetActorLocation().Z;
	float IgnoreObjectsHigherThanThis = CanvasAbove ? CanvasAbove->GetActorLocation().Z : FLT_MAX;	
	for (TActorIterator<APBCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetActorLocation().Z >= IgnoreObjectsHigherThanThis
			||
			ActorItr->GetActorLocation().Z < IgnoreObjectsLowerThanThis)
		{
			continue;
		}

		// Information of self is already written in a separate field
		if (*ActorItr == MyCharacter)
		{
			continue;
		}

		Rtn.OthersInfo.Add(GetPlayerData(*ActorItr, CanvasBelow));
	}

	for (TActorIterator<APBPlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetActorLocation().Z >= IgnoreObjectsHigherThanThis
			||
			ActorItr->GetActorLocation().Z < IgnoreObjectsLowerThanThis)
		{
			continue;
		}

		// There is many spawn points, but we only show the central ones in the mini-map
		// IMPORTANT : Level designers are responsible maintaining the bShowOnMinimap variable!
		if (!ActorItr->bShowOnMinimap)
		{
			continue;
		}

		Rtn.SpawnPointInfo.Add(GetSpawnPointData(*ActorItr, CanvasBelow));
	}

	for (TActorIterator<APBTrigger_BombSite> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetActorLocation().Z >= IgnoreObjectsHigherThanThis
			||
			ActorItr->GetActorLocation().Z < IgnoreObjectsLowerThanThis)
		{
			continue;
		}

		Rtn.BombSiteInfo.Add(GetBombSiteData(*ActorItr, CanvasBelow));
	}

	return Rtn;
}

bool UPBMiniMapWidget::GetMiniMapCanvas(const FVector& OwnerLocation, APBMiniMapCanvas*& OutCanvasBelow, APBMiniMapCanvas*& OutCanvasAbove)
{
	APBMiniMapCanvas* CanvasBelow = nullptr;
	APBMiniMapCanvas* CanvasAbove = nullptr;

	for (TActorIterator<APBMiniMapCanvas> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetActorLocation().Z <= OwnerLocation.Z)
		{
			if (CanvasBelow == nullptr
				||
				CanvasBelow->GetActorLocation().Z < ActorItr->GetActorLocation().Z)
			{
				CanvasBelow = *ActorItr;
			}
		}

		if (ActorItr->GetActorLocation().Z > OwnerLocation.Z)
		{
			if (CanvasAbove == nullptr
				||
				CanvasAbove->GetActorLocation().Z > ActorItr->GetActorLocation().Z)
			{
				CanvasAbove = *ActorItr;
			}
		}
	}

	if (CanvasBelow != nullptr)
	{
		OutCanvasBelow = CanvasBelow;
		OutCanvasAbove = CanvasAbove;
		return true;
	}
	else
	{
		return false;
	}
}

FMiniMapPlayerData UPBMiniMapWidget::GetPlayerData(APBCharacter* PlayerPawn, APBMiniMapCanvas* CanvasToProjectTo)
{
	FMiniMapPlayerData PlayerData;

	FVector RelativeLocation = PlayerPawn->GetActorLocation() - CanvasToProjectTo->GetActorLocation();
	PlayerData.Position = HorizontalProjection(RelativeLocation);

	FVector RelativeDirection = CanvasToProjectTo->GetTransform().Inverse().TransformVector(PlayerPawn->GetActorForwardVector());
	PlayerData.Direction = HorizontalProjection(RelativeDirection).GetSafeNormal();
		
	PlayerData.TimeSinceLastDetectableEvent = PlayerPawn->GetTimeSinceLastDetectableEvent();

	PlayerData.State = (!PlayerPawn->IsAlive()) ? EMiniMapPlayerState::Dead
		: (PlayerPawn->IsFiring() ? EMiniMapPlayerState::Firing : EMiniMapPlayerState::Normal);

	PlayerData.Identity = (PlayerPawn == GetOwningPlayerPawn()) ? EMiniMapPlayerIdentity::Self
		: (PlayerPawn->IsEnemyFor(GetOwningPlayer())) ? EMiniMapPlayerIdentity::Enemy : EMiniMapPlayerIdentity::Ally;

	return PlayerData;
}

FMiniMapSpawnPointData UPBMiniMapWidget::GetSpawnPointData(class APBPlayerStart* PlayerStart, APBMiniMapCanvas* CanvasToProjectTo)
{
	FMiniMapSpawnPointData SpawnPointData;

	FVector RelativeLocation = PlayerStart->GetActorLocation() - CanvasToProjectTo->GetActorLocation();
	SpawnPointData.Position = HorizontalProjection(RelativeLocation);

	SpawnPointData.Team = PlayerStart->SpawnTeam;
	return SpawnPointData;
}

FMiniMapBombSiteData UPBMiniMapWidget::GetBombSiteData(class APBTrigger_BombSite* BombSite, APBMiniMapCanvas* CanvasToProjectTo)
{
	FMiniMapBombSiteData BombSiteData;

	FVector RelativeLocation = BombSite->GetActorLocation() - CanvasToProjectTo->GetActorLocation();
	BombSiteData.Position = HorizontalProjection(RelativeLocation);

	BombSiteData.Sign = BombSite->GetSiteSign();

	BombSiteData.bBombIsSet = false; // TODO: Find out if bomb site has bomb set around it
	return BombSiteData;
}

FVector2D UPBMiniMapWidget::HorizontalProjection(const FVector& Input)
{
	return FVector2D(Input.X, Input.Y);
}

FVector2D UPBMiniMapWidget::CoordinateClamp(FVector2D Value, FVector2D Min, FVector2D Max)
{
	return FVector2D(FMath::Clamp(Value.X, Min.X, Max.X), FMath::Clamp(Value.Y, Min.Y, Max.Y));
}
