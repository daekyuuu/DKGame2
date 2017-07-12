// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBScoreAndTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBScoreAndTimerWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
	PBGetterWidgetBlueprintVariable(UTextBlock, Round);

public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

	void NotifyMatchStartedEvent(int32 Round);


	// Remaining match time
	UFUNCTION(BlueprintCallable, Category = "HUD")
		FText GetMatchTimeText() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		int32 GetMatchTime() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		FText GetTeamScoreText(EPBTeamType Team) const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		int32 GetTeamScoreNum(EPBTeamType Team) const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		EPBTeamType GetTeamType() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		float GetWinPercentage(EPBTeamType TeamType) const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		TArray<bool> GetTeamAlivilities(EPBTeamType TeamType) const;

	
};
