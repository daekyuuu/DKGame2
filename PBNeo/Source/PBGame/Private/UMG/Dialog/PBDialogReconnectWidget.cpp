// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBDialogReconnectWidget.h"

void UPBDialogReconnectWidget::Init(ULocalPlayer* PlayerOwnerArg, FText MessageTextArg, 
	FOnClicked OnReconnectedClickArg)
{
	Super::Init(PlayerOwnerArg, MessageTextArg);
	OnReconnectedClick = OnReconnectedClickArg;
}

FReply UPBDialogReconnectWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const int32 UserIndex = InKeyEvent.GetUserIndex();

	// Only ignore input from controllers that are bound to local users
	if (OwningLocalPlayer != nullptr && OwningLocalPlayer->GetGameInstance() != nullptr)
	{
		if (OwningLocalPlayer->GetGameInstance()->FindLocalPlayerFromControllerId(UserIndex))
		{
			return FReply::Unhandled();
		}
	}

	if (!InKeyEvent.IsRepeat())
	{
		if (OnReconnectedClick.IsBound())
		{
#if PLATFORM_PS4
			bool bExecute = false;

			if (OwningLocalPlayer != nullptr)
			{
				const auto OnlineSub = IOnlineSubsystem::Get();
				if (OnlineSub)
				{
					const auto IdentityInterface = OnlineSub->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						TSharedPtr<const FUniqueNetId> IncomingUserId = IdentityInterface->GetUniquePlayerId(UserIndex);
						TSharedPtr<const FUniqueNetId> DisconnectedId = OwningLocalPlayer->GetCachedUniqueNetId();

						if (*IncomingUserId == *DisconnectedId)
						{
							OwningLocalPlayer->SetControllerId(UserIndex);
							bExecute = true;
						}
					}
				}
			}
			else
			{
				bExecute = true;
			}

			if (bExecute)
			{
				return OnReconnectedClick.Execute();
			}
#else
			if (OwningLocalPlayer != nullptr)
			{
				OwningLocalPlayer->SetControllerId(UserIndex);
			}

			return OnReconnectedClick.Execute();
#endif
		}
	}

	return FReply::Unhandled();
}
