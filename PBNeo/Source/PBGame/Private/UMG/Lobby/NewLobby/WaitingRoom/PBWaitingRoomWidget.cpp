// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWaitingRoomWidget.h"

FPBWaitingRoomInfo UPBWaitingRoomWidget::GetWaitingRoomInfo()
{
	UPBWaitingRoomPage* p = GetOwningPage<class UPBWaitingRoomPage>();
	if (p)
	{
		return p->GetWaitingRoomInfo();
	}

	return FPBWaitingRoomInfo();
}

void UPBWaitingRoomWidget::OnReadyButtonPressed()
{
	UPBWaitingRoomPage* p = GetOwningPage<class UPBWaitingRoomPage>();
	if (p)
	{
		p->ReadyToPlay();
	}
}

void UPBWaitingRoomWidget::OnMapVoted(int MapIdx)
{
	UPBWaitingRoomPage* p = GetOwningPage<class UPBWaitingRoomPage>();
	if (p)
	{
		p->VoteMap(MapIdx);
	}
}
