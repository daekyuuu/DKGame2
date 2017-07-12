// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbyWidget.h"
#include "PBGameInstance.h"
#include "PBLobby.h"



UPBLobbyWidget::UPBLobbyWidget()
{
	OwningPage = nullptr;
	this->bIsFocusable = true;
}

void UPBLobbyWidget::NotifyTabIndex_Implementation(int32 Index)
{

}

void UPBLobbyWidget::PopPage()
{
	if (GetPBGameInstance())
	{
		auto LobbyInst = GetPBGameInstance()->LobbyInstance;
		if (LobbyInst)
		{
			LobbyInst->PopPage();
		}
	}
}

void UPBLobbyWidget::OnWidgetShown_Implementation()
{

}

void UPBLobbyWidget::PushPage(EPageType PageType)
{
	if (GetPBGameInstance())
	{
		auto LobbyInst = GetPBGameInstance()->LobbyInstance;
		if (LobbyInst)
		{
			LobbyInst->PushPage(PageType);
		}
	}
}
