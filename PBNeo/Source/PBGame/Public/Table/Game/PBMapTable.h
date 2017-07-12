// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Table/PBTable.h"
#include "PBMapTable.generated.h"

USTRUCT(BlueprintType)
struct FPBMapTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBMapTableData()
	{
		TableType = EPBTableType::GameMap;
	}

	// int32 ItemID; Key값 (ID 필드는 자동으로 key 값으로 들어가고 있음)
	// 네이밍은 .CSV 필드명과 동일해야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	int32 DescriptionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	TAssetPtr<class UWorld> BPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	TAssetPtr<class UTexture2D> IconResVote;	//type FString 은 임시임, Texture 나 머터리얼 타입으로 하면 될 듯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString LoadingRes;		//type FString 은 임시임, Texture 나 머터리얼 타입으로 하면 될 듯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString	MiniMapRes;		//type FString 은 임시이고... 이 변수는 테이블에 필요 없을 것 같다. yes, should we delete it? ; )
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	float VoteValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	int32 MaxPlayerCountEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString	ModeShortName;
};

typedef TMap<int32, FPBMapTableData*> FMapTableDataList;

UCLASS()
class PBGAME_API UPBMapTable : public UPBTable
{
	GENERATED_BODY()

public:
	UPBMapTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::GameMap;
	}

	TMap<FString, FMapTableDataList > ModeMaps;

	bool Load(FString TablePath) override;
	FPBMapTableData* GetData(int32 ItemID);
	FMapTableDataList* GetDataByMode(FString GameMode);
};
