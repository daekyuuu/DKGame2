// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_SoldierBtn.h"
#include "PBLoadout_SoldierTab.h"
#include "ScrollBox.h"
#include "PanelWidget.h"
#include "PBUMGUtils.h"


PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_SoldierTab, ScrollBoxRed);
PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_SoldierTab, ScrollBoxBlue);


void UPBLoadout_SoldierTab::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_ScrollBoxRed();
	GetBPV_ScrollBoxBlue();
}

void UPBLoadout_SoldierTab::NativeDestruct()
{

	Super::NativeDestruct();
}

void UPBLoadout_SoldierTab::BuildPBWidgets(EPBTeamType Type, const TArray<FPBItemBaseInfo>& ItemInfos)
{
	UScrollBox* TargetScrollBox = nullptr;
	if (Type == EPBTeamType::Alpha)
	{
		TargetScrollBox = GetBPV_ScrollBoxRed();
	}
	else if (Type == EPBTeamType::Bravo)
	{
		TargetScrollBox = GetBPV_ScrollBoxBlue();
	}

	if (nullptr == TargetScrollBox)
	{
		return;
	}

	TargetScrollBox->ClearChildren();
	LastFocusedWidget = nullptr;



	for (auto Info : ItemInfos)
	{
		AddOneSoldierBtn(TargetScrollBox, Info);
	}

	// I can do this because I know the index 0 is always SpacerWidget. and index 1 is first btn
	FocusTransferTarget = GetBPV_ScrollBoxRed()->GetChildAt(1);

	BindEvents();
}

FReply UPBLoadout_SoldierTab::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	if (LastFocusedWidget)
	{
		BroadcastDepthChanged(0);
		return TransferUserFocusThroughReply(InFocusEvent, E, LastFocusedWidget).NativeReply;
	}
	else
	{
		BroadcastDepthChanged(0);
		return TransferUserFocusThroughReply(InFocusEvent, E, FocusTransferTarget).NativeReply;
	}

	return SuperReply;
}

void UPBLoadout_SoldierTab::BroadcastDepthChanged(int32 NewDepth)
{
	auto Btns = GetSoldierBtnsInScrollBox(GetBPV_ScrollBoxRed());
	for (auto Btn : Btns)
	{
		Btn->NotifyDepthChanged(0);
	}

	Btns = GetSoldierBtnsInScrollBox(GetBPV_ScrollBoxBlue());
	for (auto Btn : Btns)
	{
		Btn->NotifyDepthChanged(0);
	}

}

void UPBLoadout_SoldierTab::BindEvents()
{

	// Bind the events of red
	auto Btns = GetSoldierBtnsInScrollBox(GetBPV_ScrollBoxRed());
	for(auto Btn : Btns)
	{
		BindPBButtonEvents<UPBLoadout_SoldierTab>(this, Btn);
	}

	// Bind the events of blue
	Btns = GetSoldierBtnsInScrollBox(GetBPV_ScrollBoxBlue());
	for (auto Btn : Btns)
	{
		BindPBButtonEvents<UPBLoadout_SoldierTab>(this, Btn);

	}

}

TArray<class UPBLoadout_SoldierBtn*> UPBLoadout_SoldierTab::GetSoldierBtnsInScrollBox(class UScrollBox* TargetScrollBox)
{
	TArray<UPBLoadout_SoldierBtn*> BtnArray;

	if (TargetScrollBox)
	{
		auto Children = UPBUMGUtils::GetChildrenOfPanel(TargetScrollBox);
		for (auto C : Children)
		{
			auto Btn = Cast<UPBLoadout_SoldierBtn>(C);
			if (Btn)
			{
				BtnArray.Add(Btn);
			}
		}
	}

	return BtnArray;
}

void UPBLoadout_SoldierTab::AddOneSoldierBtn(class UScrollBox* TargetScrollBox, const FPBItemBaseInfo& ItemInfo)
{
	if (nullptr == TargetScrollBox)
	{
		return;
	}

	if (nullptr == SoldierBtnWidgetClass)
	{
		return;
	}

	if (nullptr == PBSpacerWidgetClass)
	{
		return;
	}

	if (TargetScrollBox->GetChildrenCount() == 0)
	{
		AddOneSpacer(TargetScrollBox);
	}

	// Create the Soldier btn and add to scroll box after assign id
	auto SoldierBtn = CreateWidget<UPBLoadout_SoldierBtn>(GetOwningPlayer(), SoldierBtnWidgetClass);
	if (nullptr == SoldierBtn)
	{
		return;
	}
	SoldierBtn->AssignItemInfo(ItemInfo);
	TargetScrollBox->AddChild(SoldierBtn);

	AddOneSpacer(TargetScrollBox);

}

void UPBLoadout_SoldierTab::AddOneSpacer(class UScrollBox* TargetScrollBox)
{
	if (nullptr == TargetScrollBox)
	{
		return;
	}

	// Add Spacer widget for layout
	auto PBSpacer = CreateWidget<UPBSubWidget>(GetOwningPlayer(), PBSpacerWidgetClass);
	if (PBSpacer)
	{
		TargetScrollBox->AddChild(PBSpacer);
	}

}


void UPBLoadout_SoldierTab::HandlePBButtonReleased(class UPBUserWidget* Widget)
{
	auto SoldierBtn = Cast<UPBLoadout_SoldierBtn>(Widget);
	if (nullptr == SoldierBtn)
	{
		return;
	}

	// if it's the red team's widget
	if (true == UPBUMGUtils::IsTheWidgetInThePanel(SoldierBtn, GetBPV_ScrollBoxRed()))
	{
		CheckTheTargetAndUncheckOthers(GetBPV_ScrollBoxRed(), SoldierBtn);
	}
	// if it's the blue team's widget
	else if (true == UPBUMGUtils::IsTheWidgetInThePanel(SoldierBtn, GetBPV_ScrollBoxBlue()))
	{
		CheckTheTargetAndUncheckOthers(GetBPV_ScrollBoxBlue(), SoldierBtn);
	}

	// call the event!
	this->OnPBButtonReleased.Broadcast(Widget);

}

void UPBLoadout_SoldierTab::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	this->OnPBButtonCanceled.Broadcast(Widget);
}

void UPBLoadout_SoldierTab::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{
	if (bFocused && Widget)
	{
		FocusTransferTarget = Widget;

		LastFocusedWidget = Widget;
	}
}

void UPBLoadout_SoldierTab::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	if (nullptr == Widget)
	{
		return;
	}

	if (true == UPBUMGUtils::IsRight(PressedKey))
	{
		NavigateToNext(false);
		return;
	}

	if (true == UPBUMGUtils::IsLeft(PressedKey))
	{
		NavigateToNext(true);
		return;
	}

	if (true == UPBUMGUtils::IsUp(PressedKey))
	{
		NavigateToUp(false);
		return;
	}

	if (true == UPBUMGUtils::IsDown(PressedKey))
	{
		NavigateToUp(true);
		return;
	}
}

void UPBLoadout_SoldierTab::NavigateToNext(bool bInverseDir)
{
	TArray<UScrollBox*> ScrollBoxArray;
	ScrollBoxArray.Add(GetBPV_ScrollBoxRed());
	ScrollBoxArray.Add(GetBPV_ScrollBoxBlue());

	UPBUMGUtils::NavigateToNextInScrollBoxes(GetOwningLocalPlayer(), ScrollBoxArray, bInverseDir, OffsetForFocus, OffsetForScroll);
}

void UPBLoadout_SoldierTab::NavigateToUp(bool bInverseDir)
{
	if (bInverseDir)
	{
		UPBUMGUtils::NavigateToOtherScrollBox(GetOwningLocalPlayer(), GetBPV_ScrollBoxRed(), GetBPV_ScrollBoxBlue(), ScrollOffsetForOneStep, OffsetForFocus);
	}
	else
	{
		UPBUMGUtils::NavigateToOtherScrollBox(GetOwningLocalPlayer(), GetBPV_ScrollBoxBlue(), GetBPV_ScrollBoxRed(), ScrollOffsetForOneStep, OffsetForFocus);

	}
}

void UPBLoadout_SoldierTab::CheckTheTargetAndUncheckOthers(class UScrollBox* TargetScrollBox, class UPBUserWidget* TargetWidget)
{
	if (nullptr == TargetWidget)
	{
		return;
	}

	if (nullptr == TargetScrollBox)
	{
		return;
	}


	auto SoldierBtns = GetSoldierBtnsInScrollBox(TargetScrollBox);
	for (auto SoldierBtn : SoldierBtns)
	{
		if (SoldierBtn)
		{
			bool Res = (TargetWidget == SoldierBtn);
			SoldierBtn->SetCheck(Res);
		}
	}


}
