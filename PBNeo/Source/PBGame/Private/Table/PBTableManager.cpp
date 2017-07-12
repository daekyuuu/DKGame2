// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBTableManager.h"

UPBTableManager::UPBTableManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//Constructor(생성자) 안에서만 호출 가능합니다.
void UPBTableManager::LoadTable(EPBTableType Type, FString Path)
{
	UPBTable* Instance = CreateInstance(Type);
	if (Instance)
	{
		Instance->Load(Path);
		Tables.Add(Type, Instance);
	}
}

UPBTable* UPBTableManager::GetTable(EPBTableType Type)
{
	UPBTable** InstancePtr = Tables.Find(Type);
	UPBTable* Instance = InstancePtr ? *InstancePtr : nullptr;
	return Instance;
}

void UPBTableManager::ApplyDataToBlueprint(EPBTableType Type)
{
	UPBTable* Table = GetTable(Type);
	if (Table)
	{
		Table->ApplyDataToDefaultObjects();

		FString strTable = EnumToString(EPBTableType, Type);
		print(FString::Printf(TEXT("Sync table to [%s] Blueprint completed"), *strTable));
	}
}

