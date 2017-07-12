// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBSituationBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBSituationBoardWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);


protected:
	UFUNCTION()
	void ToggleSituationBoard();

	UFUNCTION()
	void ShowSituationBoard(bool bShow);

	UFUNCTION(BlueprintImplementableEvent, Category = "ScoreBoard")
	void ShowBeKilledUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "ScoreBoard")
	void HideBeKilledUI();

	
};
