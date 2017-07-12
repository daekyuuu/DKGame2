// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Table/PBTable.h"
#include "PBItemTable_Base.generated.h"

/**
 * 
 */

UCLASS(abstract)
class PBGAME_API UPBItemTable_Base : public UPBTable
{
	GENERATED_BODY()
		
public:
	UPBItemTable_Base(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer)
	{

	}

protected:

	bool IndexIsValid(uint32 ID);	
};
