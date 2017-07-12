// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBoardLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBoardLayoutWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "ScoreBoard")
	TArray<class APBPlayerState*> TeamPlayerArray;
	
	UFUNCTION(BlueprintCallable, Category = "ScoreBoard")
	void UpdateTeamPlayerArray(EPBTeamType teamType);

	UFUNCTION(BlueprintCallable, Category = "ScoreBoard")
	class APBPlayerState* GetTeamPlayerState(int32 index) const;

	UFUNCTION(BlueprintCallable, Category = "ScoreBoard")
		int32 GetTeamScoreNum(EPBTeamType Team) const;

};
