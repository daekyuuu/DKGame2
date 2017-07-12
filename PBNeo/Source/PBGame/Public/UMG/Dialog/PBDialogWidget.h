// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "PBGame.h"
#include "PBTypes.h"

#include "UMG/PBMainWidget.h"
#include "PBDialogWidget.generated.h"

UCLASS()
class PBGAME_API UPBDialogWidget : public UPBMainWidget
{
	GENERATED_BODY()

	/* ------------------------------------------------------------------------------------ */
	// Persist across travelling between different UWorlds

protected:
	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	/* ------------------------------------------------------------------------------------ */
	// General information every Dialog should have

public: // For BP
	UFUNCTION(BlueprintCallable, Category = Dialog)
	ULocalPlayer* GetTargetLocalPlayer();

	UFUNCTION(BlueprintCallable, Category = Dialog)
	FText GetMessageText();

protected:
	void Init(ULocalPlayer* OwningLocalPlayerArg, FText MessageTextArg);

	UPROPERTY()
	ULocalPlayer* OwningLocalPlayer;

	FText MessageText;
		
};
