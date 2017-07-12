// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBHUDWidget.generated.h"

/**
 */
UCLASS(HideDropdown, editinlinenew, Blueprintable)
class PBGAME_API UPBHUDWidget : public UPBMainWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintVariable(UTextBlock, ConnectionType);

	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Squad);
	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Weap);
	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Minimap);
	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Log);

	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Squad_Mask);
	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Minimap_Mask);
	PBGetterWidgetBlueprintVariable(UPBSubWidget, PB3D_Log_Mask);

	PBGetterWidgetBlueprintVariable(UPBSubWidget, ActionBar);

	PBGetterWidgetBlueprintVariable(UPBCrossHairWidget, CrossHair);
	PBGetterWidgetBlueprintVariable(UPBBombSiteIndicatorWidget, BombSiteIndicator);
	PBGetterWidgetBlueprintVariable(UPBHitEffectPlayerWidget, HitEffect);

public:			
	UPBHUDWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// localizing test
	UFUNCTION(BlueprintCallable, Category = "HUD")
	FText GetLocalizingTestText() const;

	void NotifyConnectionTypeChanged(bool IsServer);

protected:

	UFUNCTION()
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	UFUNCTION()
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

	void NotifyBombExploded();

};
