// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBLocalPlayer.h"
#include "OnlineSubsystemUtilsClasses.h"
#include "PBGameInstance.h"
#include "PBUserWidget.h"

UPBLocalPlayer::UPBLocalPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPBPersistentUser* UPBLocalPlayer::GetPersistentUser() const
{
	// if persistent data isn't loaded yet, load it
	if (PersistentUser == nullptr)
	{
		UPBLocalPlayer* const MutableThis = const_cast<UPBLocalPlayer*>(this);
		// casting away constness to enable caching implementation behavior
		MutableThis->LoadPersistentUser();
	}
	return PersistentUser;
}

void UPBLocalPlayer::LoadPersistentUser()
{
	// if we changed controllerid / user, then we need to load the appropriate persistent user.
	if (PersistentUser != nullptr && ( GetControllerId() != PersistentUser->GetUserIndex() || GetNickname() != PersistentUser->GetName() ) )
	{
		PersistentUser->SaveIfDirty();
		PersistentUser = nullptr;
	}

	if (PersistentUser == NULL)
	{
		// Use the platform id here to be resilient in the face of controller swapping and similar situations.
		FPlatformUserId PlatformId = GetControllerId();

		auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid() && GetPreferredUniqueNetId().IsValid())
		{
			PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetPreferredUniqueNetId());
		}

		PersistentUser = UPBPersistentUser::LoadPersistentUser( GetNickname(), PlatformId );
	}
}

TSharedPtr<SWidget> UPBLocalPlayer::GetFousedWidgetBeforeControllerInvalidated()
{
	return FocusedWidgetBeforeControllerInvalidated;
}

void UPBLocalPlayer::SetControllerId(int32 NewControllerId)
{
	// If the controller id is invalidated
	if (NewControllerId < 0)
	{
		FocusedWidgetBeforeControllerInvalidated = FSlateApplication::Get().GetUserFocusedWidget(GetControllerId());
	}

	ULocalPlayer::SetControllerId(NewControllerId);

	// if we changed controllerid / user, then we need to load the appropriate persistent user.
	if (PersistentUser != nullptr && ( GetControllerId() != PersistentUser->GetUserIndex() || GetNickname() != PersistentUser->GetName() ) )
	{
		PersistentUser->SaveIfDirty();
		PersistentUser = nullptr;
	}

	if (!PersistentUser)
	{
		LoadPersistentUser();
	}
}

FString UPBLocalPlayer::GetNickname() const
{
	FString UserNickName = Super::GetNickname();

	if ( UserNickName.Len() > MAX_PLAYER_NAME_LENGTH )
	{
		UserNickName = UserNickName.Left( MAX_PLAYER_NAME_LENGTH ) + "...";
	}

	bool bReplace = (UserNickName.Len() == 0);

	// Check for duplicate nicknames...and prevent reentry
	static bool bReentry = false;
	if(!bReentry)
	{
		bReentry = true;
		UPBGameInstance* GameInstance = GetWorld() != NULL ? Cast<UPBGameInstance>(GetWorld()->GetGameInstance()) : NULL;
		if(GameInstance)
		{
			// Check all the names that occur before ours that are the same
			const TArray<ULocalPlayer*>& LocalPlayers = GameInstance->GetLocalPlayers();
			for (int i = 0; i < LocalPlayers.Num(); ++i)
			{
				const ULocalPlayer* LocalPlayer = LocalPlayers[i];
				if( this == LocalPlayer)
				{
					break;
				}

				if( UserNickName == LocalPlayer->GetNickname())
				{
					bReplace = true;
					break;
				}
			}
		}
		bReentry = false;
	}

	if ( bReplace )
	{
		UserNickName = FString::Printf( TEXT( "Player%i" ), GetControllerId() + 1 );
	}	

	return UserNickName;
}
