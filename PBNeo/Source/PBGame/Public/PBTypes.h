// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "PBTypes.generated.h"

UENUM(BlueprintType)
enum class EPBAsyncResult : uint8
{
	Success,
	Failure,
	Pending,
};

UENUM(BlueprintType)
enum class EPBTeamType : uint8
{
	Alpha  = 0	UMETA(DisplayName = "Alpha"), // Red, Free Rebel
	Bravo		UMETA(DisplayName = "Bravo"), // Blue, CT Force
	Max,	
};

extern FString GetTeamShortName(EPBTeamType eTeamType);


/** keep in sync with PBImpactEffect */
UENUM()
namespace EPBPhysMaterialType
{
	enum Type
	{
		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
		Carpet,
		Paper,
	};
}

namespace EPBDialogType
{
	enum Type
	{
		None,
		Generic,
		ControllerDisconnected
	};
}

#define PB_SURFACE_Default		SurfaceType_Default
#define PB_SURFACE_Concrete		SurfaceType1
#define PB_SURFACE_Dirt			SurfaceType2
#define PB_SURFACE_Water		SurfaceType3
#define PB_SURFACE_Metal		SurfaceType4
#define PB_SURFACE_Wood			SurfaceType5
#define PB_SURFACE_Grass		SurfaceType6
#define PB_SURFACE_Glass		SurfaceType7
#define PB_SURFACE_Flesh		SurfaceType8
#define PB_SURFACE_Carpet		SurfaceType9
#define PB_SURFACE_Paper		SurfaceType10
#define PB_SURFACE_Head			SurfaceType11

// GameLog at right top of ingame hud
USTRUCT(BlueprintType)
struct FPBLogMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	uint8 bIsFilled : 1;

	// kill, die, assist, piercing, headshot...
	// It should be changed to some data type
	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	FString VerbString;

	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	UTexture* VerbIcon;

	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	class APBPlayerState* KillerPlayerState;
		
	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	class APBPlayerState* VictimPlayerState;

	FPBLogMessage()
		:bIsFilled(false), VerbIcon(nullptr), KillerPlayerState(nullptr), VictimPlayerState(nullptr)
	{
	}
};

extern FString GetNetRoleString(ENetRole Role);

extern FString GetNetModeString(ENetMode Mode);

UENUM(BlueprintType)
enum class EPBGameModeType : uint8
{
	TDM = 0,
	BOMB,
	Max,
};

extern FString GetGameModeString(EPBGameModeType Mode);