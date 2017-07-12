// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBInGameWidget.generated.h"

UENUM(Blueprintable)
enum class EPBInGameWidgetState : uint8
{
	WaitingToStart,
	MatchInProgress,
	WaitingPostMatch,
	WaitingConcludeAllMatches,
	None,
};

/**
 * 
 */
UCLASS()
class PBGAME_API UPBInGameWidget : public UPBMainWidget
{
	GENERATED_BODY()

public:

	UPBInGameWidget();

	void SetState(FName InMatchState, uint8 InMatchSubstateIndex);

	EPBInGameWidgetState GetState() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "InGameWidget")
	void NotifyMatchResult(bool bIsWinner);

protected:
	UFUNCTION(BlueprintCallable, Category = "Warmup")
	FText GetRamainingTime() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "InGameWidget")
	void OnStateHasChanged(EPBInGameWidgetState NewState);
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "InGameWidget")
	EPBInGameWidgetState CurrState;

};
