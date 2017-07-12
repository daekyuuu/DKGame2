// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "PBWorldSettings.generated.h"
/**
 * 
 */
UCLASS()
class PBGAME_API APBWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	APBWorldSettings();

	virtual void NotifyBeginPlay() override;

	void PlayBGM();
	void StopBGM();
	void FadeOutBGM();

	/** level music */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BGM)
	USoundCue* BGM;

protected:
	UPROPERTY()
	UAudioComponent* BGMComp;

	UPROPERTY(EditAnywhere, Category = BGM)
	float BGMFadeInTime;
	UPROPERTY(EditAnywhere, Category = BGM)
	float BGMFadeOutTime;

	//@note: EPBTeamTypeÀÇ °ªÀÌ index·Î ¸ÅÄªµÊ.
	UPROPERTY(EditDefaultsOnly, Category = TeamColor)
	TArray<FColor> TeamColors;

public:
	UFUNCTION(BlueprintCallable, Category = TeamColor)
	FColor GetTeamColor(EPBTeamType TeamType);
};
