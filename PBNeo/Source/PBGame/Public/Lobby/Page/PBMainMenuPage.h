// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Lobby/Page/PBWidgetPage.h"
#include "PBMainMenuPage.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMainMenuPage : public UPBWidgetPage
{
	GENERATED_BODY()
	
public:
	void RequestToPushThePageWithTabIndex(EPageType PageType, int32 TabIndex);
	
	
};
