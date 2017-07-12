// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBScoreWidget.h"
#include "PBPlayerState.h"
#include "PBCharacter.h"
#include "PBPlayerInfoAccessComponent.h"


void UPBScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInfoAccessComp = NewObject<UPBPlayerInfoAccessComponent>(this);
	PlayerInfoAccessComp->LocalPlayerController = (this->GetOwningPlayer());
}


void UPBScoreWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

