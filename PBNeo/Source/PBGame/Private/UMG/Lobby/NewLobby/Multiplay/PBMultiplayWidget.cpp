// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMultiplayWidget.h"

#include "PBMultiplayPage.h"
#include "PBLobbyWidget.h"


void UPBMultiplayWidget::OnModeChanged(int32 Index)
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		p->OnModeChanged(Index);
	}
}

void UPBMultiplayWidget::OnMemberChanged(int32 Index)
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		p->OnMemberChanged(Index);
	}
}

void UPBMultiplayWidget::OnTimeChanged(int32 Index)
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		p->OnTimeChanged(Index);
	}
}

void UPBMultiplayWidget::OnRoundChanged(int32 Index)
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		p->OnRoundChanged(Index);
	}
}

EPBAsyncResult UPBMultiplayWidget::OnCreateRoom()
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		return p->OnCreateRoom();
	}

	return EPBAsyncResult::Failure;
}

EPBAsyncResult UPBMultiplayWidget::OnSearchServerToJoin()
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		return p->OnSearchServerToJoin();
	}

	return EPBAsyncResult::Failure;
}

EPBAsyncResult UPBMultiplayWidget::OnConnectToServer(const FPBRoomEntry& ServerEntry)
{
	UPBMultiplayPage* p = GetOwningPage<UPBMultiplayPage>();
	if (p)
	{
		return p->OnConnectToServer(ServerEntry);
	}

	return EPBAsyncResult::Failure;
}
