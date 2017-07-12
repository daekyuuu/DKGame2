// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBPickupWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBPickupWidget : public UPBSubWidget
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UCanvasPanel, MainCanvas);

protected:


	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void Reset();


	void NotifyEnterOuterPickup(class APBPickup* Pickup);
	void NotifyLeaveOuterPickup(class APBPickup* Pickup);

	void NotifyReadyPickup(class APBPickup* Pickup);
	void NotifyStartPickup(class APBPickup* Pickup, float TimerDuration);
	void NotifyCancelPickup(class APBPickup* Pickup);
	void NotifyFinishPickup(class APBPickup* Pickup);

	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);
	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);

private:
	void BuildPickupIcon(class APBPickup* Pickup);

	void RemovePickupIcon(class APBPickup* Pickup);

	void UpdatePickupIconsLocation();

	/**
	* Hide the icon, if it is behind of character.
	* Otherwise, show the icon.
	*/
	void UpdateIconsVisibilityAccordingToRelativeLocation();

	UPBPickupIconWidget* GetIconWidgetFromContainer(class APBPickup* Pickup);

private:

	UPROPERTY()
		TMap<TWeakObjectPtr<class APBPickup>, class UPBPickupIconWidget*> PickupIcons;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
		TSubclassOf<class UPBPickupIconWidget> PickupIconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	float ScreenYOffset;


	
};
