// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMainMenuPage.h"
#include "PBLobby.h"
#include "PBWidgetPage.h"
#include "PBLobbyWidget.h"


void UPBMainMenuPage::RequestToPushThePageWithTabIndex(EPageType PageType, int32 TabIndex)
{
	auto LobbyInst = GetLobbyInstance();
	if (nullptr == LobbyInst)
	{
		return;
	}

	LobbyInst->PushPage(PageType);

	auto TargetPageData = LobbyInst->GetPageData(PageType);
	if (nullptr == TargetPageData)
	{
		return;
	}

	auto TargetPage = TargetPageData->PageInstance;
	auto CurrPage = LobbyInst->GetCurrentPage();
	if (false == (TargetPage && CurrPage))
	{
		return;
	}

	auto LobbyWidget = CurrPage->GetLobbyWidget();
	if (nullptr == LobbyWidget)
	{
		return;
	}


	LobbyWidget->NotifyTabIndex(TabIndex);


}
