// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBombSiteIndicatorWidget.generated.h"

USTRUCT()
struct FPBBombSiteIndicatorData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = BombSite)
	class APBTrigger_BombSite *TriggerBombSite;

	UPROPERTY(EditDefaultsOnly, Category = BombSite)
	class UPBBombSiteIndicatorImageWidget *IndicatorImageWidget;
};

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBombSiteIndicatorWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
	PBGetterWidgetBlueprintVariable(UCanvasPanel, Canvas);
	
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	/* -------------------------------------------------------------------------------------- */
	// Utilities
	/* -------------------------------------------------------------------------------------- */

protected:
	APBCharacter* GetOwningCharacter();

	/* -------------------------------------------------------------------------------------- */
	// BombSiteData
	/* -------------------------------------------------------------------------------------- */
private:
	TArray<FPBBombSiteIndicatorData> BombSiteData;

protected:
	UFUNCTION(BlueprintCallable, Category = "BombSite")
	TArray<FPBBombSiteIndicatorData> GetBombSiteData();

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	void UpdateBombSiteData();

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	void AddBombSiteData();

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	void RemoveBombSiteData();

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	bool OwningPlayerIsDied(APlayerState* VictimPlayerState);

protected:
	//UPROPERTY(BlueprintReadWrite, Category = "BombSite")
	//bool IsAlive;

	UPROPERTY(BlueprintReadWrite, Category = "BombSite")
	bool IsZooming;

	// 화면 테두리 옵셋값.(최대값은 0.25)
	UPROPERTY(EditDefaultsOnly, Category = "BombSite", meta = (ClampMin = 0.0, ClampMax = 0.25))
	float OffsetContractScreenBorder;

	float AngleAsDegree;

	/* -------------------------------------------------------------------------------------- */
	// Notifies
	/* -------------------------------------------------------------------------------------- */
	
public:
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

	void NotifyZoom(bool NowZooming);

	//void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	void NotifyBombPlanted();

	void NotifyBombExploded();

	void NotifyBombDefused();

protected:
	void InitIndicators();

	void TickIndicators(float InDeltaTime);

	void UpdateAlertModeIndicatorImageWidget();

	void StopAnimationOfIndicatorWidgets();
};
