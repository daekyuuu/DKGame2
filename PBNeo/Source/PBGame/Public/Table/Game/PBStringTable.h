// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Table/PBTable.h"
#include "PBStringTable.generated.h"

USTRUCT(BlueprintType)
struct FPBStringTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBStringTableData()
	{
		TableType = EPBTableType::GameString;
	}

	// ID 필드는 자동으로 key 값으로 들어가고 있음
	// 네이밍은 .CSV 필드명과 동일해야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StringData)
	FString Text;
};

/**
 * Loacalizing 관련해서 추가 작업이 필요하다. 언어에 맞게 테이블을 로드 해야 하고..
   언리얼 로컬라이징도 고려해야 하고... int32 ID 도 string 이 되어야 좀 더 인지가 쉽고. 암튼 이거는 임시다.
 */
UCLASS()
class PBGAME_API UPBStringTable : public UPBTable
{
	GENERATED_BODY()
	
public:
	UPBStringTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::GameString;
	}

	FPBStringTableData* GetData(int32 ID);	
	FString GetString(int32 ID);
};
