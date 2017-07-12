// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBCrossHairWidget.generated.h"

class APBCharacter;

/**
 * 
 */
UCLASS()
class PBGAME_API UPBCrossHairWidget : public UPBSubWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintVariable(UCanvasPanel, Canvas);

	UPBCrossHairWidget(const FObjectInitializer &ObjectInitializer);
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	/* -------------------------------------------------------------------------------------- */
	// Utilities
	/* -------------------------------------------------------------------------------------- */

protected:
	APBCharacter* GetOwningCharacter();
	
	/* -------------------------------------------------------------------------------------- */
	// Notifies
	/* -------------------------------------------------------------------------------------- */

public:
	//UFUNCTION(BlueprintNativeEvent, Category = "CrossHair")
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

	//UFUNCTION(BlueprintNativeEvent, Category = "CrossHair")
	void NotifyZoom(bool NowZooming);

	//UFUNCTION(BlueprintNativeEvent, Category = "CrossHair")
	void NotifyWeaponChange();

	//UFUNCTION(BlueprintNativeEvent, Category = "CrossHair")
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	/* -------------------------------------------------------------------------------------- */
	// Aim Point
	/* -------------------------------------------------------------------------------------- */
public:
	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	void ForceUpdateAimPointOnScreen();

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	FVector2D GetAimPointInViewport();

protected:
	void InitAimPoint();

	void UpdateAimPoint(float InDeltaTime);

	FVector GetPredictedHitLocation();

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	APBCharacter *GetViewer() const;

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	bool GetViewerIsZooming() const;

protected:
	float FirePredictionScanDistance = 1000;
	float AimPointInterpRate = 3.f;
	FVector2D InterpAimPointOnScreen;

	/* -------------------------------------------------------------------------------------- */
	// Enemy Detection
	/* -------------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	bool GetAimingAtEnemy();

	/* -------------------------------------------------------------------------------------- */
	// Expansion
	/* -------------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	float GetExpansionSize();
	

	// Crosshair Size
protected:
	float GapAngleToCrossHairGapSize(float GapAngle);

	// Corsshair Style
protected:
	void AddCrossHairImage(UClass *InCrossHairImageClass);
	void RemoveCrossHairImage();
	void UpdateCrossHairImage();

	// Crosshair Position
protected:
	void SetCrossHairPosition(const FVector2D &InLocalPosition);
	FVector2D GetLocalCentre();
	bool ShouldFixCrossHairPosition();

	/* -------------------------------------------------------------------------------------- */
	// Weapon-dependent Crosshair Setting
	/* -------------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	bool GetCrossHairPositionFixed();

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	TSubclassOf<UPBCrossHairImageWidget> GetCrossHairImageWidgetClass();

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	bool GetZoomCrossHairPositionFixed();

	UFUNCTION(BlueprintCallable, Category = "CrossHair")
	TSubclassOf<UPBCrossHairImageWidget> GetZoomCrossHairImageWidgetClass();

protected:
	UPROPERTY(BlueprintReadWrite)
	UPBCrossHairImageWidget *CrossHairImageWidgetRef;
	
	UPROPERTY(BlueprintReadWrite)
	float CrossHairConeLength;
};
