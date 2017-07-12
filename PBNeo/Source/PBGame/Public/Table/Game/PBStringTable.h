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

	// ID �ʵ�� �ڵ����� key ������ ���� ����
	// ���̹��� .CSV �ʵ��� �����ؾ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StringData)
	FString Text;
};

/**
 * Loacalizing �����ؼ� �߰� �۾��� �ʿ��ϴ�. �� �°� ���̺��� �ε� �ؾ� �ϰ�..
   �𸮾� ���ö���¡�� ����ؾ� �ϰ�... int32 ID �� string �� �Ǿ�� �� �� ������ ����. ��ư �̰Ŵ� �ӽô�.
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
