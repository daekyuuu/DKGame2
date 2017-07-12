// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Lobby/Page/PBWidgetPage.h"
#include "PBOptionPage.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBOptionPage : public UPBWidgetPage
{
	GENERATED_BODY()

public:
	UPBOptionPage();

	virtual void Init(class APlayerController* OwningPlayer, TSubclassOf<class UPBMainWidget> HUDWidgetClass) override;
	virtual void Enter() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOptionEnterDelegate, float, GammaOpt, float, AimSensitivity, bool, bInverseYAxis);
	UPROPERTY(BlueprintAssignable, Category = "Option")
		FOptionEnterDelegate OnEnter;


private:

	float GammaOpt;
	float AimSensitivityOpt;
	bool bInvertYAxisOpt;

public:

	bool SetAimSensitivity(int OptionIndex);

	bool SetGamma(int OptionIndex);

	bool SetInvertYAxis(bool Value);

	void LoadPersistentValues();

	void DoApplyChanges();



};
