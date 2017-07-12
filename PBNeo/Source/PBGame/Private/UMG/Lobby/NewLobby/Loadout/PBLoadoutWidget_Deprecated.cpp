// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadoutWidget_Deprecated.h"
#include "PBLoadoutPage.h"
#include "PBItemInfo.h"
#include "PBLoadout_SoldierTab.h"
#include "PBLoadout_WeapTab.h"


PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_SoldierTab, UPBLoadoutWidget_Deprecated, SoldierTab);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTab, UPBLoadoutWidget_Deprecated, WeaponTab);
PBGetterWidgetBlueprintVariable_Implementation(UPBLoadout_WeapTab, UPBLoadoutWidget_Deprecated, SpecialTab);


void UPBLoadoutWidget_Deprecated::NativeConstruct()
{

	auto p = GetOwningPage<UPBLoadoutPage>();
	if (p)
	{

		p->SoldierListIsReceivedEvt.AddUObject(this, &UPBLoadoutWidget_Deprecated::NotifySoldierListIsRecevied);
		p->WeapListIsReceivedEvt.AddUObject(this, &UPBLoadoutWidget_Deprecated::NotifyWeapListIsRecevied);
	}

	// NOTE: here's the point BP Construct() event is started.
	// It must be called after bind the events
	Super::NativeConstruct();

	GetBPV_SoldierTab();
	GetBPV_WeaponTab();
	GetBPV_SpecialTab();

	BindEvents();

}

void UPBLoadoutWidget_Deprecated::NativeDestruct()
{
	auto p = GetOwningPage<UPBLoadoutPage>();
	if (p)
	{
		p->SoldierListIsReceivedEvt.RemoveAll(this);
		p->WeapListIsReceivedEvt.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UPBLoadoutWidget_Deprecated::BindEvents()
{
	GetBPV_SoldierTab()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget_Deprecated::HandlePBButtonCanceled);
	GetBPV_WeaponTab()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget_Deprecated::HandlePBButtonCanceled);
	GetBPV_SpecialTab()->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLoadoutWidget_Deprecated::HandlePBButtonCanceled);

	// Other events of each tabs will be handled in Blueprint.
}

void UPBLoadoutWidget_Deprecated::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	// Go back to the main menu
	PopPage();
}

void UPBLoadoutWidget_Deprecated::OnSoldierIsSelected(int32 Id)
{
	UPBLoadoutPage* p = GetOwningPage<class UPBLoadoutPage>();
	if (p)
	{
		p->OnSoldierIsSelected(Id);
	}

}

void UPBLoadoutWidget_Deprecated::OnWeapIsSelected(int32 Id)
{
	UPBLoadoutPage* p = GetOwningPage<class UPBLoadoutPage>();
	if (p)
	{
		p->OnWeapIsSelected(Id);
	}

}

void UPBLoadoutWidget_Deprecated::RequestSoldierList(EPBTeamType Type)
{
	UPBLoadoutPage* p = GetOwningPage<class UPBLoadoutPage>();
	if (p)
	{
		p->RequestSoldierList(Type);
	}

}

void UPBLoadoutWidget_Deprecated::RequestWeapList(int32 Type)
{
	UPBLoadoutPage* p = GetOwningPage<class UPBLoadoutPage>();
	if (p)
	{
		p->RequestWeapList((EWeaponType)Type);
	}
}

