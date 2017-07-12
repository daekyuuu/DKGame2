// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMemberInfoWidget.h"
#include "PBPlayerState.h"
#include "PBCharacter.h"
#include "PBPlayerInfoAccessComponent.h"


void UPBMemberInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInfoAccessComp = NewObject<UPBPlayerInfoAccessComponent>(this);
	PlayerInfoAccessComp->LocalPlayerController = (this->GetOwningPlayer());
}


void UPBMemberInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

