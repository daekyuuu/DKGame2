// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBAnimBasedButton.h"
#include "PBUMGUtils.h"
#include "WidgetAnimation.h"

PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBAnimBasedButton, ButtonHit);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBAnimBasedButton, ButtonTitle);



UPBAnimBasedButton::UPBAnimBasedButton()
{
	bIsFocusable = true;
	MyDepth = 0;
	bInSameDepth = false;
}

void UPBAnimBasedButton::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	UPBUMGUtils::BlockUE4Navigation(this);
}


void UPBAnimBasedButton::DesignTimeConstruct_Implementation()
{
	UpdateTitleText();
}

void UPBAnimBasedButton::UpdateTitleText()
{
	GetBPV_ButtonTitle()->SetText(TitleText);

}

void UPBAnimBasedButton::Show()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPBAnimBasedButton::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPBAnimBasedButton::NotifyDepthChanged(int32 NewDepth)
{
	bInSameDepth = (MyDepth == NewDepth);
	UpdateButtonLook();
}

void UPBAnimBasedButton::NativeConstruct()
{
	Super::NativeConstruct();

	// Don't allow focusable
	GetBPV_ButtonHit()->IsFocusable = false;


	// Bind the events to the HitButton
	GetBPV_ButtonHit()->OnClicked.AddUniqueDynamic(this, &UPBAnimBasedButton::HandleButtonHitClicked);
	GetBPV_ButtonHit()->OnHovered.AddUniqueDynamic(this, &UPBAnimBasedButton::HandleButtonHitHovered);

	UpdateTitleText();

	UpdateButtonLook();

}


void UPBAnimBasedButton::NativeDestruct()
{
	Super::NativeDestruct();
}


void UPBAnimBasedButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	PlayAnimationIfNeed();

}

FReply UPBAnimBasedButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	PlaySound(PressedSound);
	UpdateButtonLook();

	OnPBButtonFocused.Broadcast(this, true);

	return SuperReply;
}

void UPBAnimBasedButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	UpdateButtonLook();

	OnPBButtonFocused.Broadcast(this, false);
}

FReply UPBAnimBasedButton::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply SuperReply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	PlaySound(PressedSound);

	const FKey KeyCode = InKeyEvent.GetKey();

	if (true == UPBUMGUtils::IsLeft(KeyCode) ||
		true == UPBUMGUtils::IsRight(KeyCode) ||
		true == UPBUMGUtils::IsUp(KeyCode) ||
		true == UPBUMGUtils::IsDown(KeyCode) ||
		true == UPBUMGUtils::IsL1(KeyCode) ||
		true == UPBUMGUtils::IsR1(KeyCode)
		)
	{
		OnPBButtonGotNaviInput.Broadcast(this, KeyCode);
		return FReply::Handled();
	}

	return SuperReply;
}

FReply UPBAnimBasedButton::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply SuperReply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (true == UPBUMGUtils::IsEnter(InKeyEvent.GetKey()))
	{
		OnPBButtonReleased.Broadcast(this);

		return FReply::Handled();
	}

	if (true == UPBUMGUtils::IsCancel(InKeyEvent.GetKey()))
	{
		OnPBButtonCanceled.Broadcast(this);

		return FReply::Handled();
	}

	return SuperReply;
}

void UPBAnimBasedButton::UpdateButtonLook()
{
	// Implement in child
}

void UPBAnimBasedButton::HandleButtonHitClicked()
{
	OnPBButtonReleased.Broadcast(this);
}

void UPBAnimBasedButton::HandleButtonHitHovered()
{
	// Just set the focus to this widget to raise the event NativeFocusReceived();
	if (bIsFocusable)
	{
		NavigateUserFocus(GetOwningLocalPlayer(), this);
	}
}

void UPBAnimBasedButton::PlayAnimationIfNeed()
{
	if (nullptr != AnimToPlay)
	{
		PlayAnimation(AnimToPlay);
		AnimToPlay = nullptr;
	}
}
