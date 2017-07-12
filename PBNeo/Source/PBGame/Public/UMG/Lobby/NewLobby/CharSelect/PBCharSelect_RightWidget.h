// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/PBLobbyWidget.h"
#include "PBCharSelect_RightWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBCharSelect_RightWidget : public UPBLobbyWidget
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UButton, BackBtn);

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindEvents();


private:
	UFUNCTION()
		void HandleBackButtonReleased();
	
	
};
