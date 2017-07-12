// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBStringTable.h"




FPBStringTableData* UPBStringTable::GetData(int32 ID)
{
	FPBStringTableData* Data = nullptr;
	Data = Table->FindRow<FPBStringTableData>(*FString::FromInt(ID), PBTABLE_KEY_STRING);
	return Data;
}

FString UPBStringTable::GetString(int32 ID)
{
	FPBStringTableData* Data = GetData(ID);
	return Data ? Data->Text : FString(TEXT(""));
}
