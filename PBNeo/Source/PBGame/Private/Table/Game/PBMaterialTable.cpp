// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMaterialTable.h"

FPBMaterialTableData* UPBMaterialTable::GetData(int32 ID)
{
	FPBMaterialTableData* Data = nullptr;
	Data = Table->FindRow<FPBMaterialTableData>(*FString::FromInt(ID), PBTABLE_KEY_STRING);
	return Data;
}

float UPBMaterialTable::GetPiercingReduceRate(int32 ID)
{
	FPBMaterialTableData* Data = GetData(ID);
	return Data ? Data->PiercingReduceRate : 0.0f;
}


