// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBInGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBInGameMenuWidget : public UPBMainWidget
{
	GENERATED_BODY()
	
public:
	UPBInGameMenuWidget(const FObjectInitializer& ObjectInitializer);

	void NativeConstruct() override;
	void Show(int32 ZOrder = 0) override;
	void Hide() override;

	// Responds to the Player Input through PlayerController
	void ToggleGameMenu();

	// Responds to the buttons of the widget itself
	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
	void OnGameResume();

	// Responds to the buttons of the widget itself
	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
	void OnGotoMainMenu();

	// Responds to the buttons of the widget itself
	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
	void OnGameQuit();

private:
	void QuitGame();

	int32 GetOwnerUserIndex() const;	

	UPBPersistentUser* GetPersistentUser() const;

	/* ------------------------------------------------------------------------------------ */
private:
	float AimSensitivityOpt;

public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
		TArray<FText> GetAimSensitivityList();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
		bool SetAimSensitivity(int OptionIndex);

	UFUNCTION(BlueprintCallable, Category = MainMenu)
		int GetAimSensitivityIndex();

	/* ------------------------------------------------------------------------------------ */
private:
	float GammaOpt;

public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
		TArray<FText> GetGammaList();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
		bool SetGamma(int OptionIndex);

	UFUNCTION(BlueprintCallable, Category = MainMenu)
		int GetGammaIndex();

	/* ------------------------------------------------------------------------------------ */
private:
	bool bInvertYAxisOpt;

public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
		bool SetInvertYAxis(bool Value);

	UFUNCTION(BlueprintCallable, Category = MainMenu)
		bool GetInvertYAxis();

	/* ------------------------------------------------------------------------------------ */
public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
		bool ApplyChanges();

	/* ------------------------------------------------------------------------------------ */
};
