// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBMemberInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMemberInfoWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "MemberInfo")
	class UPBPlayerInfoAccessComponent* PlayerInfoAccessComp;
	
};
