// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBHUDWidget.h"
#include "Player/PBPlayerState.h"
#include "Player/PBCharacter.h"
#include "CrossHair/PBCrossHairWidget.h"
#include "BombMission/PBBombSiteIndicatorWidget.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBHUDWidget, ConnectionType);

PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Squad);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Weap);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Minimap);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Log);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Squad_Mask);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Minimap_Mask);
PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, PB3D_Log_Mask);

PBGetterWidgetBlueprintVariable_Implementation(UPBSubWidget, UPBHUDWidget, ActionBar);

PBGetterWidgetBlueprintVariable_Implementation(UPBCrossHairWidget, UPBHUDWidget, CrossHair);
PBGetterWidgetBlueprintVariable_Implementation(UPBBombSiteIndicatorWidget, UPBHUDWidget, BombSiteIndicator);
PBGetterWidgetBlueprintVariable_Implementation(UPBHitEffectPlayerWidget, UPBHUDWidget, HitEffect);


UPBHUDWidget::UPBHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPBHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnConnectionTypeChanged, UPBHUDWidget::NotifyConnectionTypeChanged);
	BindWidgetEvent(OnSpawn, UPBHUDWidget::NotifySpawn);
	BindWidgetEvent(OnDeath, UPBHUDWidget::NotifyDeath);
	BindWidgetEvent(OnBombExploded, UPBHUDWidget::NotifyBombExploded);
}

void UPBHUDWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnConnectionTypeChanged);
	UnbindWidgetEvent(OnSpawn);
	UnbindWidgetEvent(OnDeath);

	Super::NativeDestruct();
}

void UPBHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FText UPBHUDWidget::GetLocalizingTestText() const
{
	const FText text = NSLOCTEXT("UMG", "LocalizationText", "{PlayerName} localizing test");
	return FText::Format(text, FText::FromString(TEXT("PulGom")));
}

void UPBHUDWidget::NotifyConnectionTypeChanged(bool IsServer)
{
	FText ConnectionTypeDesc = IsServer ? FText::FromString("Server") : FText::FromString("Client");

	//@note: HUDWidget은 GameMode별로 갈라져 각기 다른 구성을 하고 있으므로 BP에 없을 가능성이 있다.
	auto ConnectionType = GetBPV_ConnectionType();
	if (ConnectionType)
	{
		ConnectionType->SetText(ConnectionTypeDesc);
	}
}

void UPBHUDWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (KilledPlayerState == GetPBPlayerState())
	{
		GetBPV_ActionBar()->Hide();
		GetBPV_PB3D_Squad()->Hide();
		GetBPV_PB3D_Weap()->Hide();
		GetBPV_PB3D_Minimap()->Hide();
		//GetBPV_PB3D_Log()->Hide();

		GetBPV_PB3D_Squad_Mask()->Hide();
		GetBPV_PB3D_Minimap_Mask()->Hide();
		//GetBPV_PB3D_Log_Mask()->Hide();

	}
}

void UPBHUDWidget::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (SpawnedPlayerState == GetPBPlayerState())
	{
		GetBPV_ActionBar()->Show();
		GetBPV_PB3D_Squad()->Show();
		GetBPV_PB3D_Weap()->Show();
		GetBPV_PB3D_Minimap()->Show();
		//GetBPV_PB3D_Log()->Show();

		GetBPV_PB3D_Squad_Mask()->Show();
		GetBPV_PB3D_Minimap_Mask()->Show();
		//GetBPV_PB3D_Log_Mask()->Show();

		GetBPV_CrossHair()->Show();
		GetBPV_BombSiteIndicator()->Show();
		GetBPV_HitEffect()->Show();
	}
}

void UPBHUDWidget::NotifyBombExploded()
{
	GetBPV_ActionBar()->Hide();
	GetBPV_PB3D_Squad()->Hide();
	GetBPV_PB3D_Weap()->Hide();
	GetBPV_PB3D_Minimap()->Hide();
	//GetBPV_PB3D_Log()->Hide();

	GetBPV_PB3D_Squad_Mask()->Hide();
	GetBPV_PB3D_Minimap_Mask()->Hide();
	//GetBPV_PB3D_Log_Mask()->Hide();
	
	GetBPV_CrossHair()->Hide();
	GetBPV_BombSiteIndicator()->Hide();
}