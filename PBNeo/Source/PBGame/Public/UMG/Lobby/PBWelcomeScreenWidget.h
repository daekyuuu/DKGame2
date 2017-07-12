// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBNetClientInterface.h"
#include "PBWelcomeScreenWidget.generated.h"

UENUM(BlueprintType)
enum class EWelcomeScreenFailureReason : uint8
{
	None,
	Fatal,
	InvalidLicence,
	InvalidId,
	InvalidPrivilege,
};

UCLASS()
class PBGAME_API UPBWelcomeScreenWidget : public UPBMainWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
	EPBAsyncResult Welcome();

	UFUNCTION(BlueprintImplementableEvent, Category = MainMenu)
	void WelcomeDone(bool Success, EWelcomeScreenFailureReason FailureReason);

private:
	void HandleLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex);

	void HandleUserCanPlay(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

	FDelegateHandle HandleClientConnect_Handle;
	void HandleClientConnect(EPBNetClientState PrevState, EPBNetClientState NewState);

	FDelegateHandle HandleClientLogin_Handle;
	void HandleClientLogin(EPBNetClientState PrevState, EPBNetClientState NewState);

	FDelegateHandle HandleGotUserData_Handle;
	void HandleGotUserData(bool NeedUpdateNickname);
};
