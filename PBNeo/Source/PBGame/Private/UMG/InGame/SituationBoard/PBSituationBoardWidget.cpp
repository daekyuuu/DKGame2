// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSituationBoardWidget.h"
#include "FPBWidgetEventDispatcher.h"



void UPBSituationBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnToggleSituation, UPBSituationBoardWidget::ToggleSituationBoard);
	BindWidgetEvent(OnShowSituation, UPBSituationBoardWidget::ShowSituationBoard);
	BindWidgetEvent(OnDeath, UPBSituationBoardWidget::NotifyDeath);
	BindWidgetEvent(OnSpawn, UPBSituationBoardWidget::NotifySpawn);
}

void UPBSituationBoardWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnToggleSituation);
	UnbindWidgetEvent(OnShowSituation);
	UnbindWidgetEvent(OnDeath);
	UnbindWidgetEvent(OnSpawn);

	Super::NativeDestruct();
}

void UPBSituationBoardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UPBSituationBoardWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (GetPBPlayerState() && VictimPlayerState)
	{
		// If I am the victim,
		if (GetPBPlayerState() == VictimPlayerState)
		{
			ShowBeKilledUI();
		}
	}
}

void UPBSituationBoardWidget::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (GetPBPlayerState() && SpawnedPlayerState)
	{
		if (SpawnedPlayerState == GetPBPlayerState())
		{
			HideBeKilledUI();

		}
	}

}

void UPBSituationBoardWidget::ToggleSituationBoard()
{
	this->ToggleWidget();
}

void UPBSituationBoardWidget::ShowSituationBoard(bool bShow)
{
	if (bShow)
	{
		this->Show();
	}
	else
	{
		this->Hide();
	}
}
