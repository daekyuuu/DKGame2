// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSwitcherBasedButton.h"
#include "PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UWidgetSwitcher, UPBSwitcherBasedButton, ButtonStateSwitcher);
PBGetterWidgetBlueprintVariable_Implementation(UWidget, UPBSwitcherBasedButton, SameDepth_Normal);
PBGetterWidgetBlueprintVariable_Implementation(UWidget, UPBSwitcherBasedButton, SameDepth_Focused);
PBGetterWidgetBlueprintVariable_Implementation(UWidget, UPBSwitcherBasedButton, OtherDepth_Normal);
PBGetterWidgetBlueprintVariable_Implementation(UWidget, UPBSwitcherBasedButton, OtherDepth_Focused);
PBGetterWidgetBlueprintVariable_Implementation(UWidget, UPBSwitcherBasedButton, CheckedWidget);



UPBSwitcherBasedButton::UPBSwitcherBasedButton()
{
	MyDepth = 0;
}

void UPBSwitcherBasedButton::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	UPBUMGUtils::BlockUE4Navigation(this);
}

void UPBSwitcherBasedButton::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_ButtonStateSwitcher();

	GetBPV_SameDepth_Normal();
	GetBPV_SameDepth_Focused();
	GetBPV_OtherDepth_Normal();
	GetBPV_OtherDepth_Focused();
	GetBPV_CheckedWidget();

	UpdateButtonLook();

}


void UPBSwitcherBasedButton::NativeDestruct()
{
	Super::NativeDestruct();
}


void UPBSwitcherBasedButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}


FReply UPBSwitcherBasedButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	PlaySound(PressedSound);
	UpdateButtonLook();


	OnPBButtonFocused.Broadcast(this, true);


	return SuperReply;

}

void UPBSwitcherBasedButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	UpdateButtonLook();


	OnPBButtonFocused.Broadcast(this, false);
}


FReply UPBSwitcherBasedButton::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply SuperReply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	PlaySound(PressedSound);

	const FKey KeyCode = InKeyEvent.GetKey();
	

	if (true == UPBUMGUtils::IsLeft(KeyCode) ||
		true == UPBUMGUtils::IsRight(KeyCode) ||
		true == UPBUMGUtils::IsUp(KeyCode) ||
		true == UPBUMGUtils::IsDown(KeyCode))
	{
		OnPBButtonGotNaviInput.Broadcast(this, KeyCode);
		return FReply::Handled();
	}

	

	return SuperReply;
}

FReply UPBSwitcherBasedButton::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

FReply UPBSwitcherBasedButton::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	OnPBButtonReleased.Broadcast(this);

	return SuperReply;

}

FReply UPBSwitcherBasedButton::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	PlaySound(PressedSound);

	return SuperReply;
}

void UPBSwitcherBasedButton::UpdateButtonLook()
{
	auto Switcher = GetBPV_ButtonStateSwitcher();
	if (nullptr == Switcher)
	{
		return;
	}
	
	if (bInSameDepth)
	{
		if (true == ContainOwnerUserFocus())
		{
			Switcher->SetActiveWidget(GetBPV_SameDepth_Focused());
		}
		else
		{
			Switcher->SetActiveWidget(GetBPV_SameDepth_Normal());
		}
	}
	else
	{
		if (bFocusedBeforeDepthChange)
		{
			Switcher->SetActiveWidget(GetBPV_OtherDepth_Focused());
		}
		else
		{
			Switcher->SetActiveWidget(GetBPV_OtherDepth_Normal());
		}
	}

	if (bChecked)
	{
		GetBPV_CheckedWidget()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GetBPV_CheckedWidget()->SetVisibility(ESlateVisibility::Hidden);
	}


}

void UPBSwitcherBasedButton::NotifyDepthChanged(int32 NewDepth)
{
	bInSameDepth = (NewDepth == MyDepth);
	UpdateButtonLook();

}

void UPBSwitcherBasedButton::SetCheck(bool InChecked)
{
	bChecked = InChecked;
	UpdateButtonLook();

}

bool UPBSwitcherBasedButton::IsChecked() const
{
	return bChecked;
}

void UPBSwitcherBasedButton::SetFocusedBeforeDepthChange(bool bRes)
{
	bFocusedBeforeDepthChange = bRes;
	UpdateButtonLook();
}

