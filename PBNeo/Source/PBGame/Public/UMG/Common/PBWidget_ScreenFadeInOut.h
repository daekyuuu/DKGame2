// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBUserWidget.h"
#include "PBWidget_ScreenFadeInOut.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBWidget_ScreenFadeInOut : public UPBUserWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintVariable(UImage, BackgroundImage);

	UPBWidget_ScreenFadeInOut(const FObjectInitializer &ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void Reset(bool bInFadeInOrOut);
	void StartToFade(float InDelayInSeconds);
	bool IsFadingIn() const;
	bool IsFadingOut() const;

private:
	FLinearColor ColorAndOpacityCached;
	float DelayInSecondsCached;
	float OpacityChangeRaitoCached;
private:
	bool bHasConstructed;
	bool bHasStartedToFade;
	bool bEventFinished;
	float AccumulatedSeconds;
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFinishToFade, bool /* bFadeInOrNot */);
	FOnFinishToFade OnFinishToFade;
protected:
	// true: Fade In, false: Fade Out
	UPROPERTY(EditAnywhere)
	bool bFadeInOrOut;
	UPROPERTY(EditDefaultsOnly)
	float DelayInSeconds;
	UPROPERTY(EditDefaultsOnly)
	float OpacityMin;
	UPROPERTY(EditDefaultsOnly)
	float OpacityMax;
	UPROPERTY(EditDefaultsOnly)
	float OpacityChangeRaito;
};
