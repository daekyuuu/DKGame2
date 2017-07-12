// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBItemInfo.generated.h"
/**
 * 
 */

USTRUCT(Blueprintable)
struct FPBItemBaseInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PBItemInfo")
	int32 ItemId;

	UPROPERTY(BlueprintReadOnly, Category = "PBItemInfo")
	bool bLocked;

	UPROPERTY(BlueprintReadOnly, Category = "PBItemInfo")
	bool bEquipped;

	FPBItemBaseInfo()
	{
		ItemId = 0;
		bLocked = false;
		bEquipped = false;
	}
};
