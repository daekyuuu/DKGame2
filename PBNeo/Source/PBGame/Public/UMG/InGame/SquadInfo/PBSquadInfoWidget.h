// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBSquadInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBSquadInfoWidget : public UPBSubWidget
{
	GENERATED_BODY()

protected:
	RankedPlayerMap RankedSquadMembers;
	
	
	UFUNCTION(BlueprintCallable, Category = "SquadInfo")
		void UpdateRankedSquadMembers();

	UFUNCTION(BlueprintCallable, Category = "SquadInfo")
		class APBPlayerState* GetRankedSquadMember(int32 RankNum) const;
	
};
