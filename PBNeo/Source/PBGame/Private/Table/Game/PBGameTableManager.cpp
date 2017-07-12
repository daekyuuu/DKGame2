// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBGameTableManager.h"
#include "PBGameModeTable.h"
#include "PBMapTable.h"
#include "PBStringTable.h"
#include "PBMaterialTable.h"
#include "Mode/PBGameMode.h"

UPBGameTableManager::UPBGameTableManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LoadTable(EPBTableType::GameMode, TEXT("/Game/Tables/tbGameModes"));
	LoadTable(EPBTableType::GameMap, TEXT("/Game/Tables/tbMaps"));
	LoadTable(EPBTableType::GameString, TEXT("/Game/Tables/tbStrings"));
	LoadTable(EPBTableType::GameMaterial, TEXT("/Game/Tables/tbMaterials"));
}

UPBGameTableManager* UPBGameTableManager::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetGameTableManager(WorldContextObject);
}

UPBTable* UPBGameTableManager::CreateInstance(EPBTableType Type)
{
	UPBTable* Instance = nullptr;

	switch (Type)
	{
	case EPBTableType::GameMode:
		Instance = CreateDefaultSubobject<UPBGameModeTable>(TEXT("GameModeTable"));
		break;
	case EPBTableType::GameMap:
		Instance = CreateDefaultSubobject<UPBMapTable>(TEXT("GameMapTable"));
		break;
	case EPBTableType::GameString:
		Instance = CreateDefaultSubobject<UPBStringTable>(TEXT("GameStringTable"));
		break;
	case EPBTableType::GameMaterial:
		Instance = CreateDefaultSubobject<UPBMaterialTable>(TEXT("GameMaterialTable"));
		break;
	}

	return Instance;
}

FPBMaterialTableData* UPBGameTableManager::GetMaterialTableData(TEnumAsByte<EPhysicalSurface> SurfaceType)
{
	UPBMaterialTable* Table = Cast<UPBMaterialTable>(GetTable(EPBTableType::GameMaterial));
	if (Table)
	{
		FPBMaterialTableData* TableData = Table->GetData(SurfaceType);
		return TableData;
	}

	return nullptr;
}

