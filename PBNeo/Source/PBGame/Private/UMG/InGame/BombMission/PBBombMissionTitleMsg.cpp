// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBombMissionTitleMsg.h"

void UPBBombMissionTitleMsg::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnMissionRenewed, UPBBombMissionTitleMsg::NotifyMissionRenewed);
	BindWidgetEvent(OnBombPlanted, UPBBombMissionTitleMsg::NotifyBombPlanted);
}

void UPBBombMissionTitleMsg::NativeDestruct()
{
	UnbindWidgetEvent(OnMissionRenewed);
	UnbindWidgetEvent(OnBombPlanted);

	Super::NativeDestruct();
}

void UPBBombMissionTitleMsg::NotifyMissionRenewed_Implementation()
{
}

void UPBBombMissionTitleMsg::NotifyBombPlanted_Implementation()
{
}

