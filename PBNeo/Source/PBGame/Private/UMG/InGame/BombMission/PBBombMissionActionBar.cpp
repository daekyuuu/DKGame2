// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBombMissionActionBar.h"

void UPBBombMissionActionBar::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnSpawn, UPBBombMissionActionBar::NotifySpawn);
	
	BindWidgetEvent(OnCanPlantBomb, UPBBombMissionActionBar::NotifyCanPlantBomb);
	BindWidgetEvent(OnPlantingBomb, UPBBombMissionActionBar::NotifyPlantingBomb);
	BindWidgetEvent(OnBombPlantingEnd, UPBBombMissionActionBar::NotifyPlantingBombEnd);
	BindWidgetEvent(OnBombPlantingDone, UPBBombMissionActionBar::NotifyPlantingBombDone);

	BindWidgetEvent(OnCanDefuseBomb, UPBBombMissionActionBar::NotifyCanDefuseBomb);
	BindWidgetEvent(OnDefusingBomb, UPBBombMissionActionBar::NotifyDefusingBomb);
	BindWidgetEvent(OnBombDefusingEnd, UPBBombMissionActionBar::NotifyDefusingBombEnd);
	BindWidgetEvent(OnBombDefusingDone, UPBBombMissionActionBar::NotifyDefusingBombDone);
}

void UPBBombMissionActionBar::NativeDestruct()
{
	UnbindWidgetEvent(OnSpawn);
	
	UnbindWidgetEvent(OnCanPlantBomb);
	UnbindWidgetEvent(OnPlantingBomb);
	UnbindWidgetEvent(OnBombPlantingEnd);
	UnbindWidgetEvent(OnBombPlantingDone);

	UnbindWidgetEvent(OnCanDefuseBomb);
	UnbindWidgetEvent(OnDefusingBomb);
	UnbindWidgetEvent(OnBombDefusingEnd);
	UnbindWidgetEvent(OnBombDefusingDone);

	Super::NativeDestruct();
}

void UPBBombMissionActionBar::NotifyOwnerSpawned_Implementation()
{
}

void UPBBombMissionActionBar::NotifyCanPlantBomb_Implementation(bool CanPlant)
{
}

void UPBBombMissionActionBar::NotifyPlantingBomb_Implementation(float TimeToComplete)
{
}

void UPBBombMissionActionBar::NotifyPlantingBombEnd_Implementation()
{
}

void UPBBombMissionActionBar::NotifyPlantingBombDone_Implementation()
{
}

void UPBBombMissionActionBar::NotifyCanDefuseBomb_Implementation(bool CanDefuse)
{
}

void UPBBombMissionActionBar::NotifyDefusingBomb_Implementation(float TimeToComplete)
{
}

void UPBBombMissionActionBar::NotifyDefusingBombEnd_Implementation()
{
}

void UPBBombMissionActionBar::NotifyDefusingBombDone_Implementation()
{
}

void UPBBombMissionActionBar::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (SpawnedPlayerState == GetPBPlayerState())
	{
		NotifyOwnerSpawned();
	}
}