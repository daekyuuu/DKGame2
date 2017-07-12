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

	// ID 필드는 자동으로 key 값으로 들어가고 있음
	// 네이밍은 .CSV 필드명과 동일해야함
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
