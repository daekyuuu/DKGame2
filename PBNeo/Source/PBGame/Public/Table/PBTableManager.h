// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBTable.h"
#include "PBTableManager.generated.h"

/**
 * 
 */
UCLASS(abstract)
class PBGAME_API UPBTableManager : public UObject
{
	GENERATED_BODY()

public:
	UPBTableManager(const FObjectInitializer& ObjectInitializer);
		
	UPBTable* GetTable(EPBTableType Type);

	// 테이블 데이타를 블루프린트에 덮어준다(Sync)
	virtual void ApplyDataToBlueprint(EPBTableType ItemType);

	template <class T>
	void GetTableDataList(EPBTableType Type, TArray<T*>& OutDataList)
	{
		UPBTable* Table = GetTable(Type);
		if (Table)
		{
			Table->GetDataList(OutDataList);
		}
	}

protected:

	virtual UPBTable* CreateInstance(EPBTableType Type) { return nullptr;  }

	void LoadTable(EPBTableType Type, FString Path);
	
private:

	UPROPERTY()
	TMap<EPBTableType, UPBTable*> Tables;

};
