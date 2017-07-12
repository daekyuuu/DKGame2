// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBMiniMapWidget.generated.h"

UENUM(BlueprintType)
enum class EMiniMapPlayerState : uint8
{
	Normal 	UMETA(DisplayName = "Normal"),
	Firing 	UMETA(DisplayName = "Firing"),
	Dead	UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EMiniMapPlayerIdentity : uint8
{
	Self 	UMETA(DisplayName = "Self"),
	Ally 	UMETA(DisplayName = "Ally"),
	Enemy	UMETA(DisplayName = "Enemy")
};

USTRUCT(BlueprintType)
struct FMiniMapPlayerData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FVector2D Position;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FVector2D Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	float TimeSinceLastDetectableEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	EMiniMapPlayerState State;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	EMiniMapPlayerIdentity Identity;

	/** defaults */
	FMiniMapPlayerData()
	{
		Position = FVector2D(0.f, 0.f);
		Direction = FVector2D(1.f, 0.f);
		TimeSinceLastDetectableEvent = 0.f;
		State = EMiniMapPlayerState::Normal;
		Identity = EMiniMapPlayerIdentity::Ally;
	}
};

USTRUCT(BlueprintType)
struct FMiniMapSpawnPointData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	EPBTeamType Team;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FVector2D Position;

	/** defaults */
	FMiniMapSpawnPointData()
	{
		Team = EPBTeamType::Max;
		Position = FVector2D(0.f, 0.f);
	}
};

USTRUCT(BlueprintType)
struct FMiniMapBombSiteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	EPBBombSiteSign Sign;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FVector2D Position;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	bool bBombIsSet;

	/** defaults */
	FMiniMapBombSiteData()
	{
		Sign = EPBBombSiteSign::Max;
		Position = FVector2D(0.f, 0.f);
		bBombIsSet = false;
	}
};

USTRUCT(BlueprintType)
struct FMiniMapData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	bool DataIsValid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FVector2D CanvasExtent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	UTexture2D* CanvasTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	FMiniMapPlayerData	OwnInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	TArray<FMiniMapPlayerData> OthersInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	TArray<FMiniMapSpawnPointData> SpawnPointInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MiniMap)
	TArray<FMiniMapBombSiteData> BombSiteInfo;

	/** defaults */
	FMiniMapData()
	{
		DataIsValid = false;
		CanvasExtent = FVector2D(0.f, 0.f);
		CanvasTexture = nullptr;
		OwnInfo = FMiniMapPlayerData();
		OthersInfo = TArray<FMiniMapPlayerData>();
		SpawnPointInfo = TArray<FMiniMapSpawnPointData>();
		BombSiteInfo = TArray<FMiniMapBombSiteData>();
	}
};

class APBMiniMapCanvas;

UCLASS()
class PBGAME_API UPBMiniMapWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	FMiniMapData GetMiniMapData();

protected:
	// Key behavioral functions
	bool GetMiniMapCanvas(const FVector& OwnerLocation, APBMiniMapCanvas*& OutCanvasBelow, APBMiniMapCanvas*& OutCanvasAbove);
	FMiniMapPlayerData GetPlayerData(APBCharacter* PlayerPawn, APBMiniMapCanvas* CanvasToProjectTo);
	FMiniMapSpawnPointData GetSpawnPointData(class APBPlayerStart* PlayerStart, APBMiniMapCanvas* CanvasToProjectTo);
	FMiniMapBombSiteData GetBombSiteData(class APBTrigger_BombSite* BombSite, APBMiniMapCanvas* CanvasToProjectTo);

	// Utilities
	static FVector2D HorizontalProjection(const FVector& Input);
	static FVector2D CoordinateClamp(FVector2D Value, FVector2D Min, FVector2D Max);	
};
