// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBWidgetPage.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PBGAME_API UPBWidgetPage : public UObject
{
	GENERATED_BODY()

protected:

	// This widget will be used as a background 2D HUD.
	UPROPERTY()
	class UPBLobbyWidget* LobbyWidgetInst;

	UPROPERTY()
	class ULocalPlayer* LocalPlayer;

public:
	virtual void Init(class APlayerController* OwningPlayer, TSubclassOf<class UPBMainWidget> HUDWidgetClass);
	virtual void Enter();
	virtual void Exit();

protected:
	void Show();
	void Hide();

	// set the focus to initial focus widget
	void SetInitialFocus();

public:
	class UPBGameInstance* GetGameInstance() const;
	class UPBPersistentUser* GetPersistentUser() const;
	class ULocalPlayer* GetOwningLocalPlayer() const;
	class UPBLobby* GetLobbyInstance() const;
	class UPBLobbyWidget* GetLobbyWidget() const;
	
};
