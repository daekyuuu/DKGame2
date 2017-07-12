// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/PBLobbyWidget.h"
#include "PBOptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBOptionWidget : public UPBLobbyWidget
{
	GENERATED_BODY()

public:

	// event handlers

	UFUNCTION(BlueprintCallable, Category = "Option")
	void OnGammaSelectionUpdated(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Option")
	void OnAimSensitivitySelectionUpdated(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Option")
	void OnYAxisInversionUpdated(bool bIsInverse);

	UFUNCTION(BlueprintCallable, Category = "Option")
	void OnApplyChangeClicked();


	// Widget helpers

	UFUNCTION(BlueprintCallable, Category = "Option")
		TArray<FText> GetGammaList();

	UFUNCTION(BlueprintCallable, Category = "Option")
		TArray<FText> GetAimSensitivityList();

	UFUNCTION(BlueprintCallable, Category = "Option")
		int GetGammaIndex(float Value);

	UFUNCTION(BlueprintCallable, Category = "Option")
		int GetAimSensitivityIndex(float Value);





	
};
