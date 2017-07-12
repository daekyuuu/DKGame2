// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Table/PBTable.h"
#include "PBMaterialTable.generated.h"

USTRUCT(BlueprintType)
struct FPBMaterialTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBMaterialTableData()
	{
		TableType = EPBTableType::GameMaterial;
	}

	// ID �ʵ�� �ڵ����� key ������ ���� ����
	// ���̹��� .CSV �ʵ��� �����ؾ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MaterialData)
	FString MaterialName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MaterialData)
	float PiercingReduceRate;
};

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMaterialTable : public UPBTable
{
	GENERATED_BODY()
	
public:
	UPBMaterialTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::GameMaterial;
	}

	FPBMaterialTableData* GetData(int32 ID);
	float GetPiercingReduceRate(int32 ID);
};
