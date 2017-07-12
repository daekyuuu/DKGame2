// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_WeapTab.h"
#include "PBLoadout_WeapClipBtn.h"
#include "PBLoadout_WeapList.h"
#include "PBLoadout_WeapTypeList.h"
#include "PBLoadout_WeapTypeBtn.h"
#include "WidgetSwitcher.h"
#include "PBLoadout_Weap.h"
#include "PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_WeapTab, WeapClipScrollBox);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapClipBtn, UPBLoadout_WeapTab, WeapClipMain);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapClipBtn, UPBLoadout_WeapTab, WeapClipSub);

PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapList, UPBLoadout_WeapTab, WeapList);

PBGetterWidgetBlueprintVariable_Implementation(UWidgetSwitcher, UPBLoadout_WeapTab, WeapTypeListSwitcher);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeList, UPBLoadout_WeapTab, WeapTypeMain);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeList, UPBLoadout_WeapTab, WeapTypeSub);

UPBLoadout_WeapTab::UPBLoadout_WeapTab()
{
	CurrDepth = 0;
	CurrClipIndex = 0;
}

void UPBLoadout_WeapTab::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_WeapClipScrollBox();
	GetBPV_WeapClipMain();
	GetBPV_WeapClipSub();

	GetBPV_WeapList();

	GetBPV_WeapTypeListSwitcher();
	GetBPV_WeapTypeMain();
	GetBPV_WeapTypeSub();

	BindEvents();

	LastReleasedMainTypeBtn = GetBPV_WeapTypeMain()->GetBPV_Type0();
	LastReleasedSubTypeBtn = GetBPV_WeapTypeSub()->GetBPV_Type0();

}

void UPBLoadout_WeapTab::NativeDestruct()
{

	Super::NativeDestruct();
}

FReply UPBLoadout_WeapTab::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	if (LastFocusedWidget)
	{
		BroadcastDepthChanged(CurrDepth);
		return TransferUserFocusThroughReply(InFocusEvent, E, LastFocusedWidget).NativeReply;
	}
	else
	{
		BroadcastDepthChanged(CurrDepth);
		return TransferUserFocusThroughReply(InFocusEvent, E, GetBPV_WeapClipMain()).NativeReply;
	}

	return SuperReply;
}

void UPBLoadout_WeapTab::BindEvents()
{
	BindEventsOfWeapClip();

	BindEventsOfWeapType();

	BindEventsOfWeapList();
}

void UPBLoadout_WeapTab::BindEventsOfWeapClip()
{

	GetBPV_WeapClipMain()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipBtnReleased);
	GetBPV_WeapClipSub()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipBtnReleased);

	GetBPV_WeapClipMain()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipBtnCanceled);
	GetBPV_WeapClipSub()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipBtnCanceled);

	GetBPV_WeapClipMain()->OnPBButtonGotNaviInput.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipGotNaviInput);
	GetBPV_WeapClipSub()->OnPBButtonGotNaviInput.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipGotNaviInput);

	GetBPV_WeapClipMain()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipFocused);
	GetBPV_WeapClipSub()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapClipFocused);
}

void UPBLoadout_WeapTab::BindEventsOfWeapType()
{
	GetBPV_WeapTypeMain()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeReleased);
	GetBPV_WeapTypeSub()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeReleased);

	GetBPV_WeapTypeMain()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeCanceled);
	GetBPV_WeapTypeSub()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeCanceled);

	GetBPV_WeapTypeMain()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeFocused);
	GetBPV_WeapTypeSub()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapTypeFocused);
}

void UPBLoadout_WeapTab::BindEventsOfWeapList()
{
	GetBPV_WeapList()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapBtnCanceled);
	GetBPV_WeapList()->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapBtnFocused);
	GetBPV_WeapList()->OnPBButtonReleased.AddUniqueDynamic(this, &UPBLoadout_WeapTab::HandleWeapBtnReleased);


}

void UPBLoadout_WeapTab::NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget)
{
	if (nullptr == NewFocusedWidget)
	{
		return;
	}

	if (NewFocusedWidget == GetBPV_WeapClipMain())
	{
		GetBPV_WeapClipMain()->SetFocusedBeforeDepthChange(true);
		GetBPV_WeapClipSub()->SetFocusedBeforeDepthChange(false);

	}
	else if(NewFocusedWidget == GetBPV_WeapClipSub())
	{
		GetBPV_WeapClipMain()->SetFocusedBeforeDepthChange(false);
		GetBPV_WeapClipSub()->SetFocusedBeforeDepthChange(true);

	}
}

void UPBLoadout_WeapTab::HandleWeapClipBtnReleased(UPBUserWidget* Widget)
{
	// Depth 0 -> 1

	auto ClipBtn = Cast<UPBLoadout_WeapClipBtn>(Widget);
	if (nullptr == ClipBtn)
	{
		return;
	}

	NavigateToWeapTypeBtn(ClipBtn);
	

}

void UPBLoadout_WeapTab::HandleWeapClipBtnCanceled(UPBUserWidget* Widget)
{
	this->OnPBButtonCanceled.Broadcast(Widget);
}

void UPBLoadout_WeapTab::HandleWeapClipGotNaviInput(UPBUserWidget* Widget, FKey PressedKey)
{
	if (nullptr == Widget)
	{
		return;
	}

	if (true == UPBUMGUtils::IsRight(PressedKey))
	{
		NavigateToNextClip(false);
		return;
	}

	if (true == UPBUMGUtils::IsLeft(PressedKey))
	{
		NavigateToNextClip(true);
		return;
	}
}

void UPBLoadout_WeapTab::HandleWeapClipFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (false == bFocused)
	{
		return;
	}

	auto WeapClip = Cast<UPBLoadout_WeapClipBtn>(Widget);

	if (nullptr == WeapClip)
	{
		return;
	}


	if (WeapClip == GetBPV_WeapClipMain())
	{
		GetBPV_WeapTypeListSwitcher()->SetActiveWidgetIndex(MainClipIndex);
		CurrClipIndex = MainClipIndex;
	}
	else if (WeapClip == GetBPV_WeapClipSub())
	{
		GetBPV_WeapTypeListSwitcher()->SetActiveWidgetIndex(SubClipIndex);
		CurrClipIndex = SubClipIndex;
	}

	LastFocusedWidget = WeapClip;
	LastFocusedClipBtn = WeapClip;

	NotifyNewFocusToSibling(Widget);



	// NOTE: I want to show the last used weapon list If it exist.
	if (CurrClipIndex == MainClipIndex)
	{
		if (LastReleasedMainTypeBtn)
		{
			this->OnWeapTypeFocused.Broadcast(LastReleasedMainTypeBtn->GetWeapTypeId());
		}
	}
	else if (CurrClipIndex == SubClipIndex)
	{
		if (LastReleasedSubTypeBtn)
		{
			this->OnWeapTypeFocused.Broadcast(LastReleasedSubTypeBtn->GetWeapTypeId());
		}
	}

}

void UPBLoadout_WeapTab::HandleWeapTypeCanceled(UPBUserWidget* Widget)
{
	// Depth 1 -> 0
	BroadcastDepthChanged(0);
	NavigateUserFocus(GetOwningLocalPlayer(), LastFocusedClipBtn);
}

void UPBLoadout_WeapTab::HandleWeapTypeReleased(UPBUserWidget* Widget)
{

	// Depth 1 -> 2

	auto WeapTypeBtn = Cast<UPBLoadout_WeapTypeBtn>(Widget);
	if (nullptr == WeapTypeBtn)
	{
		return;
	}
	
	if (CurrClipIndex == MainClipIndex)
	{
		LastReleasedMainTypeBtn = WeapTypeBtn;
	}

	else if (CurrClipIndex == SubClipIndex)
	{
		LastReleasedSubTypeBtn = WeapTypeBtn;
	}


	NavigateToWeapListIfPossible();
	

}

void UPBLoadout_WeapTab::HandleWeapTypeFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (false == bFocused)
	{
		return;
	}

	auto WeaoTypeBtn = Cast<UPBLoadout_WeapTypeBtn>(Widget);
	if (nullptr == WeaoTypeBtn)
	{
		return;
	}

	LastFocusedWidget = WeaoTypeBtn;
	LastFocusedTypeBtn = WeaoTypeBtn;

	this->OnWeapTypeFocused.Broadcast(WeaoTypeBtn->GetWeapTypeId());
}

void UPBLoadout_WeapTab::HandleWeapBtnCanceled(UPBUserWidget* Widget)
{
	// Depth 2 -> 1
	BroadcastDepthChanged(1);
	NavigateUserFocus(GetOwningLocalPlayer(), LastFocusedTypeBtn);
}

void UPBLoadout_WeapTab::HandleWeapBtnReleased(UPBUserWidget* Widget)
{
	auto WeapBtn = Cast<UPBLoadout_Weap>(Widget);
	if (nullptr == WeapBtn)
	{
		return;
	}
	
	this->OnWeapBtnReleased.Broadcast(WeapBtn->GetItemId());
	
}

void UPBLoadout_WeapTab::HandleWeapBtnFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (bFocused)
	{
		LastFocusedWidget = Widget;
	}
}

void UPBLoadout_WeapTab::BroadcastDepthChanged(int32 NewDepth)
{
	GetBPV_WeapClipMain()->NotifyDepthChanged(NewDepth);
	GetBPV_WeapClipSub()->NotifyDepthChanged(NewDepth);

	GetBPV_WeapTypeMain()->NotifyDepthChanged(NewDepth);
	GetBPV_WeapTypeSub()->NotifyDepthChanged(NewDepth);

	GetBPV_WeapList()->NotifyDepthChanged(NewDepth);

	CurrDepth = NewDepth;

}

void UPBLoadout_WeapTab::NavigateToNextClip(bool bInverseDir)
{
	if (true == ContainUserFocus(GetOwningLocalPlayer(), GetBPV_WeapClipScrollBox()))
	{
		UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_WeapClipScrollBox(), bInverseDir);
	}
}

void UPBLoadout_WeapTab::NavigateToWeapTypeBtn(class UPBLoadout_WeapClipBtn* CallClip)
{
	if (nullptr == CallClip)
	{
		return;
	}

	UWidget* SwitchTarget = nullptr;

	if (CallClip == GetBPV_WeapClipMain())
	{
		SwitchTarget = GetBPV_WeapTypeMain();
	}
	else if (CallClip == GetBPV_WeapClipSub())
	{
		SwitchTarget = GetBPV_WeapTypeSub();
	}

	if (nullptr == SwitchTarget)
	{
		return;
	}

	GetBPV_WeapTypeListSwitcher()->SetActiveWidget(SwitchTarget);
	BroadcastDepthChanged(1);
	NavigateUserFocus(GetOwningLocalPlayer(), SwitchTarget);


}

void UPBLoadout_WeapTab::NavigateToWeapListIfPossible()
{
	if (false == GetBPV_WeapList()->IsNavigatable())
	{
		return;
	}

	BroadcastDepthChanged(2);

	NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_WeapList());
}
