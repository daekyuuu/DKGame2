// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWidget_ScreenFadeInOut.h"
#include "Components/Image.h"



PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBWidget_ScreenFadeInOut, BackgroundImage);

UPBWidget_ScreenFadeInOut::UPBWidget_ScreenFadeInOut(const FObjectInitializer &ObjectInitializer	)
	: Super(ObjectInitializer)
{
	SetEnableAutoDestroy(false);

	bHasConstructed = false;
	bHasStartedToFade = false;
	bEventFinished = false;
	bFadeInOrOut = true;
	DelayInSecondsCached = DelayInSeconds = 3.0f;
	OpacityMin = 0.0f;
	OpacityMax = 1.0f;
	OpacityChangeRaitoCached = OpacityChangeRaito = 0.75f;
	AccumulatedSeconds = 0.0f;
}

void UPBWidget_ScreenFadeInOut::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetBPV_BackgroundImage())
	{
		ColorAndOpacityCached = GetBPV_BackgroundImage()->ColorAndOpacity;
	}

	bHasConstructed = true;

	//@note: 이 아래에 작업해야함.
	Reset(bFadeInOrOut);
}

void UPBWidget_ScreenFadeInOut::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBWidget_ScreenFadeInOut::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bHasStartedToFade && false == bEventFinished && GetBPV_BackgroundImage())
	{
		AccumulatedSeconds += InDeltaTime;
		if (AccumulatedSeconds < DelayInSeconds)
			return;

		FLinearColor Color = GetBPV_BackgroundImage()->ColorAndOpacity;
		float InceasingSign = bFadeInOrOut ? -1 : 1;
		float NewOpaticy = FMath::Clamp(Color.A + (InDeltaTime * OpacityChangeRaito * InceasingSign), OpacityMin, OpacityMax);
		if (bFadeInOrOut)
		{
			// Broadcast for Fade In
			if (NewOpaticy < OpacityMin)
			{
				OnFinishToFade.Broadcast(true);
				bEventFinished = true;
			}
		}
		else
		{
			// Broadcast for Fade Out
			if (NewOpaticy > OpacityMax)
			{
				OnFinishToFade.Broadcast(false);
				bEventFinished = true;
			}
		}
		GetBPV_BackgroundImage()->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, NewOpaticy));
	}
}

void UPBWidget_ScreenFadeInOut::Reset(bool bInFadeInOrOut)
{
	if (false == bHasConstructed)
		return;

	bHasStartedToFade = false;
	bEventFinished = false;
	bFadeInOrOut = bInFadeInOrOut;
	if (GetBPV_BackgroundImage())
	{
		ColorAndOpacityCached = GetBPV_BackgroundImage()->ColorAndOpacity;
		FLinearColor NewOpacity(ColorAndOpacityCached.R, ColorAndOpacityCached.G, ColorAndOpacityCached.B, bFadeInOrOut ? 1.0f : 0.0f);
		GetBPV_BackgroundImage()->SetColorAndOpacity(NewOpacity);
	}
	DelayInSeconds = DelayInSecondsCached;
	OpacityChangeRaito = OpacityChangeRaitoCached;
	AccumulatedSeconds = 0.0f;
}

void UPBWidget_ScreenFadeInOut::StartToFade(float InDelayInSeconds)
{
	bHasStartedToFade = true;
	DelayInSeconds = InDelayInSeconds;
}

bool UPBWidget_ScreenFadeInOut::IsFadingIn() const
{
	return bHasStartedToFade && bFadeInOrOut;
}

bool UPBWidget_ScreenFadeInOut::IsFadingOut() const
{
	return bHasStartedToFade && false == bFadeInOrOut;
}