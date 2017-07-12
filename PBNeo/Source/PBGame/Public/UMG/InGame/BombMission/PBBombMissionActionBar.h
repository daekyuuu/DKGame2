// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBombMissionActionBar.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBombMissionActionBar : public UPBSubWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyOwnerSpawned();

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyCanPlantBomb(bool CanPlant);

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyPlantingBomb(float TimeToComplete);

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyPlantingBombEnd();

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyPlantingBombDone();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyCanDefuseBomb(bool CanDefuse);

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyDefusingBomb(float TimeToComplete);

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyDefusingBombEnd();

	UFUNCTION(BlueprintNativeEvent, Category = "ActionBar")
	void NotifyDefusingBombDone();

protected:

	UFUNCTION()
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

};
