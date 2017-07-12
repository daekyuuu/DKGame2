// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Table/PBTable.h"
#include "Engine/DataTable.h"
#include "PBGameModeTable.generated.h"

USTRUCT(BlueprintType)
struct FPBGameModeTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBGameModeTableData()
	{
		TableType = EPBTableType::GameMode;
	}
	void LoadDefaultObject() override;
	void ApplyDataToDefaultObject(int32 ID) override;

	// ID 필드는 자동으로 key 값으로 들어가고 있음
	// 네이밍은 .CSV 필드명과 동일해야함

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 NameStringsId;	// mode display name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	TSubclassOf<class APBGameMode> BPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	FString ModeShortName;	// 엔진에서 사용하는 short name (DefaultGame.ini 의 [/Script/Engine.GameMode] 에서 사용하는 이름을 적는다)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 ModeType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 DescriptionId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 WinRedStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 WinBlueStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 CreatePriority;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 RespawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	float PlayTimeExpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 KillCountExpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 AssistCountExpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	float PlayTimeGoldRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	float KillCountGoldRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	float AssistCountGoldRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 DefaultTimeEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 DefaultPlayerCountEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 DefaultWinCountEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 MinTimeEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	int32 MaxTimeEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameModeData)
	bool isRecord;
};

/**
 * 
 */
UCLASS()
class PBGAME_API UPBGameModeTable : public UPBTable
{
	GENERATED_BODY()
	
public:
	UPBGameModeTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::GameMode;
	}

	FPBGameModeTableData* GetData(int32 ItemID);
	void ApplyDataTo(int32 ItemID, class APBGameMode* GameMode);
	
};
