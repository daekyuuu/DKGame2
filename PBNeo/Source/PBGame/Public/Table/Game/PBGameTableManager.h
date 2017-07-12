// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Table/PBTableManager.h"
#include "PBGameTableManager.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBGameTableManager : public UPBTableManager
{
	GENERATED_BODY()
public:
	UPBGameTableManager(const FObjectInitializer& ObjectInitializer);

	static UPBGameTableManager* Get(UObject* WorldContextObject);

	struct FPBMaterialTableData* GetMaterialTableData(TEnumAsByte<EPhysicalSurface> SurfaceType);

protected:

	virtual UPBTable* CreateInstance(EPBTableType Type) override;

};

