// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_WeapList.h"
#include "PanelWidget.h"
#include "ScrollBox.h"
#include "PBPlayerController.h"
#include "PBLoadout_WeapVector4.h"
#include "PBItemInfo.h"
#include "PBLoadout_Weap.h"
#include "PBUMGUtils.h"
#include "PBLoadout_WeapVector4.h"

PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_WeapList, RootScrollBox);

UPBLoadout_WeapList::UPBLoadout_WeapList()
{
	LastFocusedWeapId = 0;
}

void UPBLoadout_WeapList::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_RootScrollBox();

}

void UPBLoadout_WeapList::NativeDestruct()
{
	Super::NativeDestruct();

}

void UPBLoadout_WeapList::BuildWeapList(const TArray<FPBItemBaseInfo>& WeapInfos, TSubclassOf<class UPBLoadout_WeapVector4> WeapVector4Class)
{
	BuildPBWidgets(WeapInfos, WeapVector4Class);

	BindEvents();
	
	// If there's last focused widget before,
	auto LastFocusedWeap = FindWeapFromId(LastFocusedWeapId);
	SetFocusTransferTarget(LastFocusedWeap);

	NotifyNewFocusToSibling(LastFocusedWeap);
	


}

bool UPBLoadout_WeapList::IsNavigatable()
{
	return (GetBPV_RootScrollBox()->GetChildrenCount() > 0);
}

void UPBLoadout_WeapList::NotifyDepthChanged(int32 NewDepth)
{
	auto Rows = GetRows();
	for (auto WeapVec4 : Rows)
	{
		if (WeapVec4)
		{
			WeapVec4->NotifyDepthChanged(NewDepth);
		}
	}
}

void UPBLoadout_WeapList::NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget)
{
	if (nullptr == NewFocusedWidget)
	{
		return;
	}

	auto Rows = GetRows();
	for (auto WeapVec4 : Rows)
	{
		if (WeapVec4)
		{
			WeapVec4->NotifyNewFocusToSibling(NewFocusedWidget);
		}
	}
}

void UPBLoadout_WeapList::BindEvents()
{
	auto Rows = GetRows();
	for (auto WeapVec4 : Rows)
	{
		if (WeapVec4)
		{
			BindPBButtonEvents<UPBLoadout_WeapList>(this, WeapVec4);
		}
	}
}



void UPBLoadout_WeapList::HandlePBButtonReleased(UPBUserWidget* Widget)
{
	if (nullptr == Widget)
	{
		return;
	}

	// Uncheck all of the weap btns except the current released btn
	auto Rows = GetRows();
	for (auto WeapVec4 : Rows)
	{
		if (WeapVec4)
		{
			WeapVec4->CheckTheTargetAndUncheckOthers(Widget);
		}
	}

	this->OnPBButtonReleased.Broadcast(Widget);

}

void UPBLoadout_WeapList::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	if (nullptr == Widget)
	{
		return;
	}

	this->OnPBButtonCanceled.Broadcast(Widget);
}

void UPBLoadout_WeapList::HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (bFocused && Widget)
	{
		// Save the row which is containing the focus
		int32 FocusContainingRowIndex = UPBUMGUtils::GetFocusWidgetIndexInThePanel(GetOwningLocalPlayer(), GetBPV_RootScrollBox());
		if (FocusContainingRowIndex != -1)
		{
			RowContainingFocus = Cast<UPBLoadout_WeapVector4>(GetBPV_RootScrollBox()->GetChildAt(FocusContainingRowIndex));
		}

		SetLastFocusedWeapId(Widget);


		this->OnPBButtonFocused.Broadcast(Widget, bFocused);
	}
}

void UPBLoadout_WeapList::HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey)
{
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
		NavigateToUp(true);
		return;
	}
	if (true == UPBUMGUtils::IsDown(PressedKey))
	{
		NavigateToUp(false);
		return;
	}

}

void UPBLoadout_WeapList::BuildPBWidgets(const TArray<FPBItemBaseInfo>& WeapInfos, TSubclassOf<class UPBLoadout_WeapVector4> WeapVector4Class)
{
	if (GetBPV_RootScrollBox() && WeapVector4Class)
	{

		// Sort as descending order
		TArray<FPBItemBaseInfo> SortedInfos = WeapInfos;
		SortedInfos.Sort([](const FPBItemBaseInfo& Lhs, const FPBItemBaseInfo& Rhs) ->bool
		{
			return Lhs.ItemId > Rhs.ItemId;
		}
		);

		GetBPV_RootScrollBox()->ClearChildren();
		int32 NumOfVector = (SortedInfos.Num() / 4) + 1;

		auto OwningPlayer = GetOwningPlayer();
		if (OwningPlayer)
		{

			for (int32 i = 0; i < NumOfVector; ++i)
			{
				UPBLoadout_WeapVector4* VectorWidget = CreateWidget<UPBLoadout_WeapVector4>(OwningPlayer, WeapVector4Class);
				if (VectorWidget)
				{
					TArray<FPBItemBaseInfo> IdsOfVectorElements;

					for (int j = 0; j < 4; ++j)
					{
						if (SortedInfos.Num() > 0)
						{
							// set inverse order of before one
							IdsOfVectorElements.Add(SortedInfos.Pop());
						}
						else
						{
							break;
						}
					}

					VectorWidget->BuildPBWidgets(IdsOfVectorElements);
					GetBPV_RootScrollBox()->AddChild(VectorWidget);
				}
			}
		}

	}
}

FReply UPBLoadout_WeapList::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// Make sure to scroll widgets before to transfer the focus to the target.
	if (FocusTransferTarget)
	{
		UPBUMGUtils::ScrollWidgetIntoViewWithOffset(GetBPV_RootScrollBox(), FocusTransferTarget, false);
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}


void UPBLoadout_WeapList::SetFocusTransferTarget(UPBUserWidget* Widget)
{
	if (Widget)
	{
		FocusTransferTarget = Widget;
	}
	else
	{
		// just set to first
		auto WeapVec4 = Cast<UPBLoadout_WeapVector4>(GetBPV_RootScrollBox()->GetChildAt(0));
		if (WeapVec4)
		{
			FocusTransferTarget = WeapVec4->GetBPV_Column0();
		}
	}

	
}

void UPBLoadout_WeapList::SetLastFocusedWeapId(UPBUserWidget* Widget)
{
	auto WeapBtn = Cast<UPBLoadout_Weap>(Widget);
	if (WeapBtn)
	{
		LastFocusedWeapId = WeapBtn->GetItemId();
	}
}

void UPBLoadout_WeapList::NavigateToNext(bool bInverseDir)
{
	if (nullptr == RowContainingFocus)
	{
		return;
	}

	auto WeapBtn = Cast<UPBLoadout_Weap>(UPBUMGUtils::FindNextInHorizontalBox(GetOwningLocalPlayer(), RowContainingFocus->GetBPV_Row0(), bInverseDir));
	if (WeapBtn)
	{
		if (WeapBtn->IsNavigatable())
		{
			NavigateUserFocus(GetOwningLocalPlayer(), WeapBtn);
		}
	}
}

void UPBLoadout_WeapList::NavigateToUp(bool bInverseDir)
{
	if (nullptr == RowContainingFocus)
	{
		return;
	}

	// 1. Get the next row in the scroll box
	auto NextRow = Cast<UPBLoadout_WeapVector4>(UPBUMGUtils::GetNextWidgetInPanel(GetBPV_RootScrollBox(), RowContainingFocus, bInverseDir));
	if (nullptr == NextRow)
	{
		return;
	}


	// 2. Scroll to next row before to navigate the focus
	UPBUMGUtils::ScrollWidgetIntoViewWithOffset(GetBPV_RootScrollBox(), NextRow, true, bInverseDir);

	// 3. Navigate the focus to current focused row's Columns[index]
	int32 FocusedColumnIndex = RowContainingFocus->GetFocusedColumnIndex();
	NextRow->NavigateFocusAt(FocusedColumnIndex);

}

class UPBLoadout_Weap* UPBLoadout_WeapList::FindWeapFromId(int32 Id)
{
	if (Id == 0)
	{
		return nullptr;
	}

	auto Rows = GetRows();
	for (auto WeapVec4 : Rows)
	{
		auto Widget = WeapVec4->FindWeapFromId(Id);
		if (Widget)
		{
			return Widget;
		}
	}

	return nullptr;
}

TArray<UPBLoadout_WeapVector4*> UPBLoadout_WeapList::GetRows()
{
	TArray<UPBLoadout_WeapVector4*> Rows;
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_RootScrollBox());
	for (auto C : Children)
	{
		auto WeapVec4 = Cast<UPBLoadout_WeapVector4>(C);
		if (WeapVec4)
		{
			Rows.Add(WeapVec4);
		}
	}

	return Rows;

}
