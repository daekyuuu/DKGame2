// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBTable.h"


//This function can be called only within the Constructor(»ý¼ºÀÚ)
bool UPBTable::Load(FString TablePath)
{	
	ConstructorHelpers::FObjectFinder<UDataTable> TableOb(*TablePath);
	Table = TableOb.Object;
	return Table != nullptr;
}


void UPBTable::LoadDefaultObjects()
{
	for (auto& Elem : Table->RowMap)
	{
		FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
		if (Data)
		{
			Data->LoadDefaultObject();
		}
	}
}

void UPBTable::ApplyDataToDefaultObjects()
{
	for (auto& Elem : Table->RowMap)
	{
		FPBTableDataBase* Data = reinterpret_cast<FPBTableDataBase*>(Elem.Value);
		if (Data)
		{
			Data->ApplyDataToDefaultObject(FCString::Atoi(*Elem.Key.ToString()));
		}
	}
}



void UPBTable::GetAllItemIds(TArray<int32>& OutIdArray)
{
	OutIdArray.Empty();
	if (Table)
	{
		auto Names = Table->GetRowNames();
		for (auto Name : Names)
		{
			OutIdArray.Add(NameToInt(Name));
		}
	}
}