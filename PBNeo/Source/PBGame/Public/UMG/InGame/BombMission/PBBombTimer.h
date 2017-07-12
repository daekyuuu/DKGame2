// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBBombTimer.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBBombTimer : public UPBSubWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintVariable(UTextBlock, CountDownTimer);

public:
	UPBBombTimer();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void NotifyBombPlanted();
	void NotifySyncBombRemainingTime(float RemainingTime);
	void NotifyBombDefused();
	void NotifyBombExploded();
	void NotifyBombFrozen();
	
protected:
	//@note: Level 4�� Loop�� �ƴϹǷ�, ����� �Ϸ�Ǹ� �� �Լ��� ȣ��ǵ����Ѵ�.
	void OnCountdownLevel5(UAudioComponent *HasFinishedAudioComponent);
	void StopBombTimer();
	void StopAllSound();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue *CountdownLevel1Sound;
	UAudioComponent *CountdownLevel1AudioComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue *CountdownLevel2Sound;
	UAudioComponent *CountdownLevel2AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue *CountdownLevel3Sound;
	UAudioComponent *CountdownLevel3AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue *CountdownLevel4Sound;
	UAudioComponent *CountdownLevel4AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue *CountdownLevel5Sound;
	UAudioComponent *CountdownLevel5AudioComponent;

private:
	float TimeToExplosion;
	int32 PrevCoundDownLevel;
};
