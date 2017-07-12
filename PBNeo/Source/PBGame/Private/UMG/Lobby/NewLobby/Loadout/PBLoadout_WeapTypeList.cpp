// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_WeapTypeList.h"
#include "PBLoadout_WeapTypeBtn.h"
#include "ScrollBox.h"
#include "PanelWidget.h"
#include "PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_WeapTypeList, WeapTypeScrollBox);

PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeBtn, UPBLoadout_WeapTypeList, Type0);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeBtn, UPBLoadout_WeapTypeList, Type1);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeBtn, UPBLoadout_WeapTypeList, Type2);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeBtn, UPBLoadout_WeapTypeList, Type3);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTypeBtn, UPBLoadout_WeapTypeList, Type4);

UPBLoadout_WeapTypeList::UPBLoadout_WeapTypeList()
{
	LastFocusedWidget = nullptr;
}

void UPBLoadout_WeapTypeList::NativeConstruct()
{
	Super::NativeConstruct();


	GetBPV_WeapTypeScrollBox();
	GetBPV_Type0();
	GetBPV_Type1();
	GetBPV_Type2();
	GetBPV_Type3();
	GetBPV_Type4();

	InitTypeBtns();

	// '1' is comes from spacer widget in UMG.
	ShrinkContentAccordingToTextArray(GetBPV_WeapTypeScrollBox(), TypeTexts, 1);

	BindEvents();

	// set the initial focus target.
	FocusTransferTarget = GetBPV_Type0();


}

void UPBLoadout_WeapTypeList::SynchronizeProperties()
{
	
	InitTypeBtns();


	Super::SynchronizeProperties();
}



void UPBLoadout_WeapTypeList::NotifyDepthChanged(int32 NewDepth)
{
	for (auto Btn : TypeBtns)
	{
		Btn->NotifyDepthChanged(NewDepth);
	}
}

void UPBLoadout_WeapTypeList::InitTypeBtns()
{
	InitTypeBtnArray();
	InitTypeBtnTexts();
	InitTypeBtnIds();
}

void UPBLoadout_WeapTypeList::InitTypeBtnArray()
{
	TypeBtns.Empty();

	TypeBtns.Add(GetBPV_Type0());
	TypeBtns.Add(GetBPV_Type1());
	TypeBtns.Add(GetBPV_Type2());
	TypeBtns.Add(GetBPV_Type3());
	TypeBtns.Add(GetBPV_Type4());


}

void UPBLoadout_WeapTypeList::InitTypeBtnTexts()
{
	for (int i = 0; i < TypeTexts.Num(); ++i)
	{
		if (TypeBtns.IsValidIndex(i) && TypeBtns[i])
		{
			TypeBtns[i]->SetButtonText(TypeTexts[i]);
		}
	}
}

void UPBLoadout_WeapTypeList::InitTypeBtnIds()
{
	for (int i = 0; i < TypeEnums.Num(); ++i)
	{
		if (TypeBtns.IsValidIndex(i) && TypeBtns[i])
		{
			TypeBtns[i]->SetWeapTypeId((int32)TypeEnums[i]);
		}
	}
}

void UPBLoadout_WeapTypeList::BindEvents()
{
	for (UPBLoadout_WeapTypeBtn* Btn : TypeBtns)
	{
		BindPBButtonEvents<UPBLoadout_WeapTypeList>(this, Btn);
	}
}

void UPBLoadout_WeapTypeList::NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget)
{
	if (nullptr == NewFocusedWidget)
	{
		return;
	}

	for (auto TypeBtn : TypeBtns)
	{
		bool bFocusedWidget = TypeBtn == NewFocusedWidget;
		TypeBtn->SetFocusedBeforeDepthChange(bFocusedWidget);
		
	
	}
}

int32 UPBLoadout_WeapTypeList::ShrinkContentAccordingToTextArray(class UPanelWidget* Panel, const TArray<FText>& TextArray, int32 RemoveOffset)
{
	if (Panel)
	{
		int32 Gap = Panel->GetChildrenCount() - (TextArray.Num() * (1 + RemoveOffset));

		while (Gap > 0)
		{
			int32 Index = Panel->GetChildrenCount() - 1;
			Panel->RemoveChildAt(Index);
			--Gap;
		}

		return Panel->GetChildrenCount();
	}

	return 0;
}

void UPBLoadout_WeapTypeList::HandlePBButtonReleased(UPBUserWidget* Widget)
{
	this->OnPBButtonReleased.Broadcast(Widget);
}

void UPBLoadout_WeapTypeList::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	this->OnPBButtonCanceled.Broadcast(Widget);
}

void UPBLoadout_WeapTypeList::HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (bFocused && Widget)
	{
		LastFocusedWidget = Widget;
		FocusTransferTarget = Widget;

		NotifyNewFocusToSibling(Widget);

		this->OnPBButtonFocused.Broadcast(Widget, true);
	}
}

void UPBLoadout_WeapTypeList::HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey)
{
	if (true == UPBUMGUtils::IsUp(PressedKey))
	{
		UPBUMGUtils::NavigateToNextInScrollBox(GetOwningLocalPlayer(), GetBPV_WeapTypeScrollBox(), true, 1, 0);
	}

	else if (true == UPBUMGUtils::IsDown(PressedKey))
	{
		UPBUMGUtils::NavigateToNextInScrollBox(GetOwningLocalPlayer(), GetBPV_WeapTypeScrollBox(), false, 1, 0);

	}
	
}
