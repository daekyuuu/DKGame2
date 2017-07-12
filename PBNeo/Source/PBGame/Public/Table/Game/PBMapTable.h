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

	// int32 ItemID; Key�� (ID �ʵ�� �ڵ����� key ������ ���� ����)
	// ���̹��� .CSV �ʵ��� �����ؾ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	int32 DescriptionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	TAssetPtr<class UWorld> BPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	TAssetPtr<class UTexture2D> IconResVote;	//type FString �� �ӽ���, Texture �� ���͸��� Ÿ������ �ϸ� �� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString LoadingRes;		//type FString �� �ӽ���, Texture �� ���͸��� Ÿ������ �ϸ� �� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapData)
	FString	MiniMapRes;		//type FString �� �ӽ��̰�... �� ������ ���̺� �ʿ� ���� �� ����. yes, should we delete it? ; )
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
