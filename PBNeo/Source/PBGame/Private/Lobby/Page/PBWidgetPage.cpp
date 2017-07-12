// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWidgetPage.h"

#include "PBMainWidget.h"
#include "PBLocalPlayer.h"
#include "PBPersistentUser.h"
#include "PBLobbyWidget.h"
#include "PBGameInstance.h"

void UPBWidgetPage::Init(class APlayerController* OwningPlayer, TSubclassOf<UPBMainWidget> HUDWidgetClass)
{
	if (nullptr == OwningPlayer)
	{
		return;
	}
	LocalPlayer = OwningPlayer->GetLocalPlayer();

	// 1.create HUD widget.
	if (HUDWidgetClass && !LobbyWidgetInst)
	{
		LobbyWidgetInst = CreateWidget<UPBLobbyWidget>(OwningPlayer, HUDWidgetClass);
	}

	if (nullptr == LobbyWidgetInst)
	{
		return;
	}

	LobbyWidgetInst->SetOwningLocalPlayer(OwningPlayer->GetLocalPlayer());
	LobbyWidgetInst->SetOwningPage(this);

	// 4. Hide UI as a default visibility
	Hide();
}

void UPBWidgetPage::Enter()
{
	Show();
	SetInitialFocus();
}

void UPBWidgetPage::Exit()
{
	Hide();
}

void UPBWidgetPage::Show()
{
	if (LobbyWidgetInst)
	{
		LobbyWidgetInst->Show();
	}

}

void UPBWidgetPage::Hide()
{
	if (LobbyWidgetInst)
	{
		LobbyWidgetInst->Hide();
	}

}

void UPBWidgetPage::SetInitialFocus()
{
	if (LocalPlayer && LobbyWidgetInst)
	{
		ULocalPlayer* LP = LocalPlayer;

		if (LP)
		{
			UPBUserWidget::NavigateUserFocus(LP, LobbyWidgetInst);
		}


	}
}

class UPBGameInstance* UPBWidgetPage::GetGameInstance() const
{
	if (LocalPlayer)
	{
		return Cast<UPBGameInstance>(LocalPlayer->GetGameInstance());
	}

	return nullptr;
}

UPBPersistentUser* UPBWidgetPage::GetPersistentUser() const
{
	if (LocalPlayer)
	{
		UPBLocalPlayer* PBLocalPlayer = Cast<UPBLocalPlayer>(GetOwningLocalPlayer());
		return PBLocalPlayer ? PBLocalPlayer->GetPersistentUser() : nullptr;

	}

	return nullptr;
}

ULocalPlayer* UPBWidgetPage::GetOwningLocalPlayer() const
{
	return LocalPlayer;
}

class UPBLobby* UPBWidgetPage::GetLobbyInstance() const
{
	if (GetGameInstance())
	{
		return GetGameInstance()->LobbyInstance;
	}

	return nullptr;
}

class UPBLobbyWidget* UPBWidgetPage::GetLobbyWidget() const
{
	return LobbyWidgetInst;
}
