// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_WeapVector4.h"
#include "PBLoadout_Weap.h"
#include "HorizontalBox.h"

PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Weap, UPBLoadout_WeapVector4, Column0);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Weap, UPBLoadout_WeapVector4, Column1);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Weap, UPBLoadout_WeapVector4, Column2);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_Weap, UPBLoadout_WeapVector4, Column3);

PBGetterWidgetBlueprintVariable_Implementation(UHorizontalBox, UPBLoadout_WeapVector4, Row0);


UPBLoadout_WeapVector4::UPBLoadout_WeapVector4()
{

}

void UPBLoadout_WeapVector4::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_Column0();
	GetBPV_Column1();
	GetBPV_Column2();
	GetBPV_Column3();

	GetBPV_Row0();
}

void UPBLoadout_WeapVector4::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBLoadout_WeapVector4::BuildPBWidgets(const TArray<FPBItemBaseInfo>& Infos)
{
	BuildPropertyArray();

	FPBItemBaseInfo EmptyInfo;

	for (int32 i = 0; i < Columns.Num(); ++i)
	{
		auto WeapBtn = Columns[i];
		if (nullptr == WeapBtn)
		{
			continue;
		}

		if (Infos.IsValidIndex(i))
		{
			WeapBtn->AssignItemInfo(Infos[i]);
		}
		else
		{
			WeapBtn->AssignItemInfo(EmptyInfo);
		}

		
	}

	BindEvents();

}

void UPBLoadout_WeapVector4::CheckTheTargetAndUncheckOthers(class UPBUserWidget* TargetWidget)
{
	if (nullptr == TargetWidget)
	{
		return;
	}

	for (auto C : Columns)
	{
		if (C != TargetWidget)
		{
			C->SetCheck(false);
		}
		else
		{
			C->SetCheck(true);
		}
	}
}

class UPBLoadout_Weap* UPBLoadout_WeapVector4::FindWeapFromId(int32 Id)
{
	for (auto C : Columns)
	{
		if (C->GetItemId() == Id)
		{
			return C;
		}
	}

	return nullptr;
}

int32 UPBLoadout_WeapVector4::GetFocusedColumnIndex()
{
	return UPBUMGUtils::GetFocusWidgetIndexInThePanel(GetOwningLocalPlayer(), GetBPV_Row0());
}

void UPBLoadout_WeapVector4::NavigateFocusAt(int32 Index)
{
	if (Index > -1)
	{
		auto WeapBtn = Cast<UPBLoadout_Weap>(GetBPV_Row0()->GetChildAt(Index));
		if (WeapBtn)
		{
			if (WeapBtn->IsNavigatable())
			{
				NavigateUserFocus(GetOwningLocalPlayer(), WeapBtn);
			}
		}
	}
}

void UPBLoadout_WeapVector4::NotifyDepthChanged(int32 NewDepth)
{
	for (auto C : Columns)
	{
		C->NotifyDepthChanged(NewDepth);
	}
}

void UPBLoadout_WeapVector4::NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget)
{
	for (auto C : Columns)
	{
		bool bFocusedWidget = (C == NewFocusedWidget);
		C->SetFocusedBeforeDepthChange(bFocusedWidget);
	}
}

void UPBLoadout_WeapVector4::BuildPropertyArray()
{
	Columns.Empty();
	Columns.Add(GetBPV_Column0());
	Columns.Add(GetBPV_Column1());
	Columns.Add(GetBPV_Column2());
	Columns.Add(GetBPV_Column3());

}

void UPBLoadout_WeapVector4::BindEvents()
{
	for (auto WeapBtn : Columns)
	{
		BindPBButtonEvents<UPBLoadout_WeapVector4>(this, WeapBtn);
	}

}

void UPBLoadout_WeapVector4::HandlePBButtonReleased(UPBUserWidget* Widget)
{
	this->OnPBButtonReleased.Broadcast(Widget);
}

void UPBLoadout_WeapVector4::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	this->OnPBButtonCanceled.Broadcast(Widget);
}

void UPBLoadout_WeapVector4::HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused)
{
	this->OnPBButtonFocused.Broadcast(Widget, bFocused);
}

void UPBLoadout_WeapVector4::HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey)
{
	this->OnPBButtonGotNaviInput.Broadcast(Widget, PressedKey);
}

