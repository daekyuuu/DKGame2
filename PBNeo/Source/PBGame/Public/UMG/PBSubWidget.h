// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBUserWidget.h"
#include "PBSubWidget.generated.h"

/**
 * 자식으로 추가하는 경우 사용.(Crosshair, Scoreboard, WeaponInfo, minimap,..)
 */
UCLASS()
class PBGAME_API UPBSubWidget : public UPBUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	virtual void Show();
	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	virtual void Hide();
	
	UFUNCTION(BlueprintCallable, Category = PBSubWidget)
	virtual bool IsOpened();

	UFUNCTION(BlueprintCallable, Category = Widget)
	virtual	void ToggleWidget();

	


};
