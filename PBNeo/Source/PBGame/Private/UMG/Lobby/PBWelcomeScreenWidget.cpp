// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWelcomeScreenWidget.h"
#include "PBGameInstance.h"
#include "PBGameViewportClient.h"

EPBAsyncResult UPBWelcomeScreenWidget::Welcome()
{
	const auto OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			ensure(GetOwningLocalPlayer());
			const auto LoginStatus = IdentityInterface->GetLoginStatus(GetOwningLocalPlayer()->GetControllerId());
			if (LoginStatus == ELoginStatus::NotLoggedIn)
			{
				const auto ExternalUI = OnlineSub->GetExternalUIInterface();
				if (ExternalUI.IsValid())
				{
					ExternalUI->ShowLoginUI(GetOwningLocalPlayer()->GetControllerId(), false, FOnLoginUIClosedDelegate::CreateUObject(this, &UPBWelcomeScreenWidget::HandleLoginUIClosed));
				}
			}
			else if (LoginStatus == ELoginStatus::LoggedIn)
			{
				TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(GetOwningLocalPlayer()->GetControllerId());
				HandleLoginUIClosed(UserId, GetOwningLocalPlayer()->GetControllerId());
			}
			return EPBAsyncResult::Pending;
		}
	}

	ensure(false);
	return EPBAsyncResult::Failure;
}

void UPBWelcomeScreenWidget::HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex)
{
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();
	if (!bIsLicensed)
	{
		WelcomeDone(false, EWelcomeScreenFailureReason::InvalidLicence);
		return;
	}

	if (!UniqueId->IsValid() || ControllerIndex < 0)
	{
		WelcomeDone(false, EWelcomeScreenFailureReason::InvalidId);
		return;
	}

	if (GetOwningLocalPlayer())
	{
		GetOwningLocalPlayer()->SetCachedUniqueNetId(UniqueId);
	}

	const auto OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			IdentityInterface->GetUserPrivilege(*UniqueId, EUserPrivileges::CanPlay, IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UPBWelcomeScreenWidget::HandleUserCanPlay));
			return;
		}
	}

	ensure(false);
	WelcomeDone(false, EWelcomeScreenFailureReason::Fatal);
	return;
}

void UPBWelcomeScreenWidget::HandleUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	if (GetPBGameInstance())
	{
		if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
		{
			UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
			ensure(ClientInterface);

			print("Connecting to server...");
			ClientInterface->ConnectToServer("192.168.2.21", 36100);
			HandleClientConnect_Handle = ClientInterface->OnStateChangedEvt.AddUObject(this, &UPBWelcomeScreenWidget::HandleClientConnect);			
			return;
		}
		else
		{
			WelcomeDone(false, EWelcomeScreenFailureReason::InvalidPrivilege);
			return;
		}
	}

	ensure(false);
	WelcomeDone(false, EWelcomeScreenFailureReason::Fatal);
	return;
}

void UPBWelcomeScreenWidget::HandleClientConnect(EPBNetClientState PrevState, EPBNetClientState NewState)
{
	if (PrevState == EPBNetClientState::NotConnected && NewState == EPBNetClientState::Connected)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
		ensure(ClientInterface);

		ClientInterface->OnStateChangedEvt.Remove(HandleClientConnect_Handle);

		print("Logging in...");
		ensure(GetOwningLocalPlayer() && GetOwningLocalPlayer()->GetCachedUniqueNetId().IsValid());
		ClientInterface->Login(GetOwningLocalPlayer()->GetCachedUniqueNetId()->ToString(), FString());
		HandleClientLogin_Handle = ClientInterface->OnStateChangedEvt.AddUObject(this, &UPBWelcomeScreenWidget::HandleClientLogin);
	}
}

void UPBWelcomeScreenWidget::HandleClientLogin(EPBNetClientState PrevState, EPBNetClientState NewState)
{
	if (PrevState == EPBNetClientState::Connected && NewState == EPBNetClientState::LoggedIn)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
		ensure(ClientInterface);

		ClientInterface->OnStateChangedEvt.Remove(HandleClientLogin_Handle);

		print("Getting UserInfo...");
		ClientInterface->GetUserInfo();
		HandleGotUserData_Handle = ClientInterface->OnGotUserInfoEvt.AddUObject(this, &UPBWelcomeScreenWidget::HandleGotUserData);
	}
}

void UPBWelcomeScreenWidget::HandleGotUserData(bool NeedUpdateNickname)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
	ensure(ClientInterface);

	ClientInterface->OnStateChangedEvt.Remove(HandleGotUserData_Handle);

	if (NeedUpdateNickname)
	{
		ensure(GetOwningLocalPlayer() && GetOwningLocalPlayer()->GetCachedUniqueNetId().IsValid());
		ClientInterface->UpdateNickname(GetOwningLocalPlayer()->GetCachedUniqueNetId()->ToString());
	}
	else
	{
		// TEMP : Update nickname all the time
		ensure(GetOwningLocalPlayer() && GetOwningLocalPlayer()->GetCachedUniqueNetId().IsValid());
		ClientInterface->UpdateNickname(GetOwningLocalPlayer()->GetCachedUniqueNetId()->ToString());
	}

	print("Enter Lobby...");
	GetPBGameInstance()->GotoState(PBGameInstanceState::MainMenu);
	WelcomeDone(true, EWelcomeScreenFailureReason::None);
}
