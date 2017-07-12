// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Engine/Datatable.h"
#include "PBTable.generated.h"

/**
*
*/

#define PBTABLE_KEY_STRING FString(TEXT("ID"))

UENUM()
enum class EPBTableType : uint8
{
	None,
	Weapon,
	Character,
	GameMode,
	GameMap,
	GameString,
	GameMaterial,
};

USTRUCT(BlueprintType)
struct FPBTableDataBase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBTableDataBase()	{}

	virtual void LoadDefaultObject() {};
	virtual void ApplyDataToDefaultObject(int32 ID) {}

public:
	EPBTableType TableType;
};


UCLASS(abstract)
class UPBTable : public UObject
{
	GENERATED_BODY()

public:
	UPBTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer)
	{
		TableType = EPBTableType::None;
		Table = nullptr;
	}

	UFUNCTION()
	virtual bool Load(FString TablePath);
	UFUNCTION()
	EPBTableType GetType() const { return TableType; }

	void LoadDefaultObjects();
	void ApplyDataToDefaultObjects();

	template <class T>
	void GetDataList(TArray<T*>& OutDataList)
	{
		Table->GetAllRows<T>(PBTABLE_KEY_STRING, OutDataList);
	}

	void GetAllItemIds(TArray<int32>& OutIdArray);


protected:
	UPROPERTY()
	UDataTable* Table;
	UPROPERTY()
	EPBTableType TableType;
};
